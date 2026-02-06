/*
* This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ChooseRpgTargetAction.h"

#include <cmath>
#include <random>
#include <ctime>

#include "BattlegroundMgr.h"
#include "ChatHelper.h"
#include "Event.h"
#include "Formations.h"
#include "Log.h"
#include "Playerbots.h"
#include "RpgSubActions.h"
#include "ServerFacade.h"
#include "PossibleRpgTargetsValue.h"
#include "Util.h"
#include "TravelMgr.h"

bool ChooseRpgTargetAction::HasSameTarget(ObjectGuid guid, uint32 max, GuidVector const& nearGuids)
{
    if (botAI->HasRealPlayerMaster())
        return false;

    uint32 num = 0;

    for (auto& i : nearGuids)
    {
        Player* player = ObjectAccessor::FindPlayer(i);
        if (!player)
            continue;

        PlayerbotAI* botAI = GET_PLAYERBOT_AI(player);
        if (!botAI)
            continue;

        if (!botAI->AllowActivity(GRIND_ACTIVITY))
            continue;

        if (PAI_VALUE(GuidPosition, "rpg target") != guid)
            continue;

        num++;
        if (num >= max)
            break;
    }

    return num > 0;
}

float ChooseRpgTargetAction::getMaxRelevance(GuidPosition guidP)
{
    GuidPosition currentRpgTarget = AI_VALUE(GuidPosition, "rpg target");
    SET_AI_VALUE(GuidPosition, "rpg target", guidP);

    Strategy* rpgStrategy = botAI->GetAiObjectContext()->GetStrategy("rpg");
    if (!rpgStrategy)
        return 0.0f;

    std::vector<TriggerNode*> triggerNodes;
    rpgStrategy->InitTriggers(triggerNodes);

    float maxRelevance = 0.0f;

    for (auto triggerNode : triggerNodes)
    {
        Trigger* trigger = botAI->GetAiObjectContext()->GetTrigger(triggerNode->getName());

        if (trigger)
        {
            triggerNode->setTrigger(trigger);

            if (triggerNode->getFirstRelevance() < maxRelevance || triggerNode->getFirstRelevance() > 2.0f)
                continue;

            Trigger* activeTrigger = triggerNode->getTrigger();

            if (!activeTrigger->IsActive())
                continue;

            NextAction** nextActions = triggerNode->getHandlers();

            bool isRpg = false;

            for (uint32 i = 0; nextActions && nextActions[i]; ++i)
            {
                NextAction* nextAction = nextActions[i];
                Action* action = botAI->GetAiObjectContext()->GetAction(nextAction->getName());
                if (dynamic_cast<RpgEnabled*>(action))
                    isRpg = true;
            }
            NextAction::destroy(nextActions);

            if (isRpg)
            {
                maxRelevance = triggerNode->getFirstRelevance();
                rpgActionReason[guidP] = triggerNode->getName();
            }
        }
    }

    for (auto trigger : triggerNodes)
        delete trigger;
    triggerNodes.clear();

    SET_AI_VALUE(GuidPosition, "rpg target", currentRpgTarget);

    if (!maxRelevance)
        return 0.0f;

    return floor((maxRelevance - 1.0f) * 1000.0f);
}

bool ChooseRpgTargetAction::Execute(Event /*event*/)
{
    Player* master = botAI->GetMaster();
    GuidPosition masterRpgTarget;
    if (master && master != bot && GET_PLAYERBOT_AI(master) && master->GetMapId() == bot->GetMapId() &&
        !master->IsBeingTeleported())
    {
        Player* player = botAI->GetMaster();
        (void)player;
    }
    else
        master = nullptr;

    std::unordered_map<ObjectGuid, uint32> targets;
    GuidVector possibleTargets = AI_VALUE(GuidVector, "possible rpg targets");
    GuidVector possibleObjects = AI_VALUE(GuidVector, "nearest game objects no los");
    GuidVector possiblePlayers = AI_VALUE(GuidVector, "nearest friendly players");
    GuidSet& ignoreList = AI_VALUE(GuidSet&, "ignore rpg target");

    for (auto target : possibleTargets)
        targets[target] = 0;

    for (auto target : possibleObjects)
        targets[target] = 0;

    for (auto target : possiblePlayers)
        targets[target] = 0;

    if (targets.empty())
        return false;

    if (urand(0, 9))
    {
        for (auto target : ignoreList)
            targets.erase(target);
    }

    SET_AI_VALUE(std::string, "next rpg action", this->getName());

    bool hasGoodRelevance = false;

    for (auto& target : targets)
    {
        Unit* unit = ObjectAccessor::GetUnit(*bot, target.first);
        if (!unit)
            continue;

        GuidPosition guidP(unit);
        if (!guidP || !guidP.getMap())
            continue;

        if (guidP.GetWorldObject() && !isFollowValid(bot, guidP.GetWorldObject()))
            continue;

        if (guidP.IsGameObject())
        {
            GameObject* go = guidP.GetGameObject();
            if (!go || !go->isSpawned() || go->GetGoState() != GO_STATE_READY)
                continue;
        }
        else if (guidP.IsPlayer())
        {
            Player* player = guidP.GetPlayer();
            if (!player)
                continue;

            if (GET_PLAYERBOT_AI(player))
            {
                GuidPosition guidPP = PAI_VALUE(GuidPosition, "rpg target");
                if (guidPP.IsPlayer())
                    continue;
            }
        }

        float relevance = getMaxRelevance(guidP);

        if (!hasGoodRelevance || relevance > 1)
            target.second = relevance;

        if (target.second > 1)
            hasGoodRelevance = true;
    }

    SET_AI_VALUE(std::string, "next rpg action", "");

    for (auto it = targets.begin(); it != targets.end();)
    {
        if (it->second == 0)
            it = targets.erase(it);
        else if (hasGoodRelevance && it->second <= 1.0f)
            it = targets.erase(it);
        else if (!hasGoodRelevance && master && (!masterRpgTarget || it->first != masterRpgTarget))
            it = targets.erase(it);
        else
            ++it;
    }

    if (targets.empty())
    {
        TC_LOG_DEBUG("playerbots", "{} can't choose RPG target: all {} targets are not available",
                     bot->GetName().c_str(), possibleTargets.size());
        RESET_AI_VALUE(GuidSet&, "ignore rpg target");
        RESET_AI_VALUE(GuidPosition, "rpg target");
        return false;
    }

    std::vector<GuidPosition> guidps;
    std::vector<int32> relevances;

    for (auto& target : targets)
    {
        Unit* unit = ObjectAccessor::GetUnit(*bot, target.first);
        if (!unit)
            continue;

        GuidPosition guidP(unit);
        if (!guidP)
            continue;

        guidps.push_back(guidP);
        relevances.push_back(target.second);
    }

    std::mt19937 gen(time(nullptr));
    TravelMgr::instance().weighted_shuffle(guidps.begin(), guidps.end(), relevances.begin(), relevances.end(), gen);

    GuidPosition guidP(guidps.front());
    if (!guidP)
    {
        RESET_AI_VALUE(GuidPosition, "rpg target");
        return false;
    }

    if (botAI->HasStrategy("debug", BOT_STATE_NON_COMBAT) && guidP.GetWorldObject())
    {
        std::ostringstream out;
        out << "found: " << ChatHelper::FormatWorldobject(guidP.GetWorldObject()) << " " << relevances.front();
        botAI->TellMasterNoFacing(out);
    }

    SET_AI_VALUE(GuidPosition, "rpg target", guidP);

    return true;
}

bool ChooseRpgTargetAction::isUseful()
{
    if (!botAI->AllowActivity(RPG_ACTIVITY))
        return false;

    GuidPosition guidP = AI_VALUE(GuidPosition, "rpg target");
    if (guidP && guidP.distance(bot) < sPlayerbotAIConfig->reactDistance * 2)
        return false;

    if (AI_VALUE(GuidVector, "possible rpg targets").empty())
        return false;

    if (!AI_VALUE(bool, "can move around"))
        return false;

    return true;
}

bool ChooseRpgTargetAction::isFollowValid(Player* bot, WorldObject* target)
{
    if (!target)
        return false;

    return isFollowValid(bot, WorldPosition(target));
}

bool ChooseRpgTargetAction::isFollowValid(Player* bot, WorldPosition pos)
{
    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
    Player* groupLeader = botAI->GetGroupLeader();
    Player* realMaster = botAI->GetMaster();

    bool inDungeon = false;

    if (botAI->HasActivePlayerMaster())
    {
        if (realMaster->IsInWorld() && realMaster->GetMap()->IsDungeon() && bot->GetMapId() == realMaster->GetMapId())
            inDungeon = true;

        if (realMaster && realMaster->IsInWorld() && realMaster->GetMap()->IsDungeon() &&
            (realMaster->GetMapId() != pos.getMapId()))
            return false;
    }

    if (!groupLeader || bot == groupLeader)
        return true;

    if (!botAI->HasStrategy("follow", BOT_STATE_NON_COMBAT))
        return true;

    if (bot->GetDistance(groupLeader) > sPlayerbotAIConfig->rpgDistance * 2)
        return false;

    Formation* formation = botAI->GetAiObjectContext()->GetValue<Formation*>("formation")->Get();
    float distance = groupLeader->GetDistance2d(pos.getX(), pos.getY());

    if (!botAI->HasActivePlayerMaster() && distance < 50.0f)
    {
        WorldPosition lastLongMove = botAI->GetAiObjectContext()->GetValue<WorldPosition>("last long move")->Get();
        if ((groupLeader && !groupLeader->isMoving()) ||
            lastLongMove.distance(pos) < sPlayerbotAIConfig->reactDistance)
            return true;
    }

    if ((inDungeon || !groupLeader->HasPlayerFlag(PLAYER_FLAGS_RESTING)) && realMaster == groupLeader &&
        distance > 5.0f)
        return false;

    if (!groupLeader->isMoving() && distance < 25.0f)
        return true;

    if (distance < formation->GetMaxDistance())
        return true;

    return false;
}

bool ClearRpgTargetAction::Execute(Event /*event*/)
{
    RESET_AI_VALUE(GuidPosition, "rpg target");
    return true;
}

bool ClearRpgTargetAction::isUseful() { return AI_VALUE(GuidPosition, "rpg target"); }
