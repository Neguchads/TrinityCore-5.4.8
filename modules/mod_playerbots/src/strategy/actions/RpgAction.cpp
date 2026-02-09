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

#include "RpgAction.h"

#include <algorithm>
#include <random>

#include "ChatHelper.h"
#include "Event.h"
#include "Formations.h"
#include "Playerbots.h"
#include "RpgSubActions.h"
#include "ServerFacade.h"

bool RpgAction::Execute(Event /*event*/)
{
    GuidPosition guidP = AI_VALUE(GuidPosition, "rpg target");
    if (!guidP && botAI->GetMaster())
    {
        if (WorldObject* target = ObjectAccessor::GetWorldObject(*bot, botAI->GetMaster()->GetTarget()))
        {
            guidP = GuidPosition(target);
            if (guidP)
            {
                RemIgnore(guidP);
                SET_AI_VALUE(GuidPosition, "rpg target", guidP);
            }
        }
    }

    if (bot->GetShapeshiftForm() > 0)
        bot->SetShapeshiftForm(FORM_NONE);

    if (!SetNextRpgAction())
        RESET_AI_VALUE(GuidPosition, "rpg target");

    return true;
}

bool RpgAction::isUseful() { return AI_VALUE(GuidPosition, "rpg target"); }

bool RpgAction::SetNextRpgAction()
{
    std::vector<Action*> actions;
    std::vector<uint32> relevances;
    std::vector<TriggerNode*> triggerNodes;

    for (auto const& strategyName : botAI->GetAiObjectContext()->GetSupportedStrategies())
    {
        if (strategyName.find("rpg") == std::string::npos)
            continue;

        Strategy* rpgStrategy = botAI->GetAiObjectContext()->GetStrategy(strategyName);
        if (!rpgStrategy)
            continue;

        rpgStrategy->InitTriggers(triggerNodes);

        for (auto& triggerNode : triggerNodes)
        {
            Trigger* trigger = botAI->GetAiObjectContext()->GetTrigger(triggerNode->getName());
            if (!trigger)
                continue;

            triggerNode->setTrigger(trigger);

            NextAction** nextActions = triggerNode->getHandlers();

            bool isChecked = false;
            for (uint32 i = 0; nextActions && nextActions[i]; ++i)
            {
                NextAction* nextAction = nextActions[i];
                if (nextAction->getRelevance() > 5.0f)
                    continue;

                if (!isChecked && !trigger->IsActive())
                    break;

                isChecked = true;

                Action* action = botAI->GetAiObjectContext()->GetAction(nextAction->getName());
                if (!dynamic_cast<RpgEnabled*>(action) || !action->isPossible() || !action->isUseful())
                    continue;

                actions.push_back(action);
                relevances.push_back(static_cast<uint32>((nextAction->getRelevance() - 1.0f) * 500.0f));
            }

            NextAction::destroy(nextActions);
        }

        for (auto* node : triggerNodes)
            delete node;
        triggerNodes.clear();
    }

    if (actions.empty())
        return false;

    if (botAI->HasStrategy("debug rpg", BOT_STATE_NON_COMBAT))
    {
        std::vector<std::pair<Action*, uint32>> sortedActions;
        for (size_t i = 0; i < actions.size(); ++i)
            sortedActions.push_back(std::make_pair(actions[i], relevances[i]));

        std::sort(sortedActions.begin(), sortedActions.end(),
            [](std::pair<Action*, uint32> const& i, std::pair<Action*, uint32> const& j) { return i.second > j.second; });

        std::stringstream ss;
        ss << "------" << ChatHelper::FormatWorldobject(AI_VALUE(GuidPosition, "rpg target").GetWorldObject()) << "------";
        bot->Say(ss.str(), LANG_UNIVERSAL);
        botAI->TellMasterNoFacing(ss.str());

        for (auto const& action : sortedActions)
        {
            std::ostringstream out;
            out << " " << action.first->getName() << " " << action.second;
            botAI->TellMasterNoFacing(out);
        }
    }

    std::mt19937 gen(static_cast<uint32>(time(nullptr)));
    TravelMgr::instance().weighted_shuffle(actions.begin(), actions.end(), relevances.begin(), relevances.end(), gen);

    Action* action = actions.front();

    if (botAI->HasStrategy("debug", BOT_STATE_NON_COMBAT) || botAI->HasStrategy("debug rpg", BOT_STATE_NON_COMBAT))
    {
        std::ostringstream out;
        out << "do: " << ChatHelper::FormatWorldobject(AI_VALUE(GuidPosition, "rpg target").GetWorldObject())
            << " " << action->getName();
        botAI->TellMasterNoFacing(out);
    }

    SET_AI_VALUE(std::string, "next rpg action", action->getName());
    return true;
}

bool RpgAction::AddIgnore(ObjectGuid guid)
{
    if (HasIgnore(guid))
        return false;

    GuidSet& ignoreList = context->GetValue<GuidSet&>("ignore rpg target")->Get();
    ignoreList.insert(guid);

    if (ignoreList.size() > 50)
        ignoreList.erase(ignoreList.begin());

    context->GetValue<GuidSet&>("ignore rpg target")->Set(ignoreList);
    return true;
}

bool RpgAction::RemIgnore(ObjectGuid guid)
{
    if (!HasIgnore(guid))
        return false;

    GuidSet& ignoreList = context->GetValue<GuidSet&>("ignore rpg target")->Get();
    ignoreList.erase(ignoreList.find(guid));
    context->GetValue<GuidSet&>("ignore rpg target")->Set(ignoreList);
    return true;
}

bool RpgAction::HasIgnore(ObjectGuid guid)
{
    GuidSet& ignoreList = context->GetValue<GuidSet&>("ignore rpg target")->Get();
    if (ignoreList.empty())
        return false;

    return ignoreList.find(guid) != ignoreList.end();
}

bool CRpgAction::isUseful()
{
    RESET_AI_VALUE(GuidPosition, "rpg target");
    return true;
}
