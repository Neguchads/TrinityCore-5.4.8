#include "GrindTargetValue.h"

#include "ChatHelper.h"
#include "Playerbots.h"
#include "QuestDef.h"
#include "SharedDefines.h"
#include "ServerFacade.h"

Unit* GrindTargetValue::Calculate()
{
    uint32 memberCount = 1;
    Group* group = bot->GetGroup();
    if (group)
        memberCount = group->GetMembersCount();

    Unit* target = nullptr;
    uint32 assistCount = 0;
    while (!target && assistCount < memberCount)
    {
        target = FindTargetForGrinding(assistCount++);
    }

    return target;
}

Unit* GrindTargetValue::FindTargetForGrinding(uint32 assistCount)
{
    uint32 memberCount = 1;
    Group* group = bot->GetGroup();
    Player* master = GetMaster();

    if (master && (master == bot || master->GetMapId() != bot->GetMapId() || master->IsBeingTeleported() || !GET_PLAYERBOT_AI(master)))
        master = nullptr;

    GuidVector attackers = context->GetValue<GuidVector>("attackers")->Get();
    for (ObjectGuid const guid : attackers)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        return unit;
    }

    GuidVector targets = *context->GetValue<GuidVector>("possible targets");
    if (targets.empty())
        return nullptr;

    float distance = 0;
    Unit* result = nullptr;
    // std::unordered_map<uint32, bool> needForQuestMap;

    for (ObjectGuid const guid : targets)
    {
        Unit* unit = botAI->GetUnit(guid);

        if (!unit)
            continue;

        if (unit->isDead()) continue;

        auto& rep = bot->ToPlayer()->GetReputationMgr();
        if (unit->ToCreature() && !unit->ToCreature()->GetCreatureTemplate()->lootid &&
            bot->GetReactionTo(unit) >= REP_NEUTRAL)
        {
            continue;
        }

        if (!bot->IsHostileTo(unit) && unit->GetNpcFlags() != UNIT_NPC_FLAG_NONE)
        {
            continue;
        }

        /*if (!bot->isHonorOrXPTarget(unit))
        {
            continue;
        }*/

        if (abs(bot->GetPositionZ() - unit->GetPositionZ()) > INTERACTION_DISTANCE)
            continue;

        if (!bot->InBattleground() && GetTargetingPlayerCount(unit) > assistCount)
            continue;

        // if (!bot->InBattleground() && master && master->GetDistance(unit) >= sPlayerbotAIConfig->grindDistance &&
        // !sRandomPlayerbotMgr->IsRandomBot(bot)) continue;

        // Bots in bot-groups no have a more limited range to look for grind target
        if (!bot->InBattleground() && master && botAI->HasStrategy("follow", BotState::BOT_STATE_NON_COMBAT)
            && sServerFacade->GetDistance2d(master, unit) > sPlayerbotAIConfig->lootDistance)
        {
            /*if (botAI->HasStrategy("debug grind", BotState::BOT_STATE_NON_COMBAT))
                botAI->TellMaster(ChatHelper::FormatWorldobject(unit) + " ignored (far from master).");
            continue;*/
        }

        if (!bot->InBattleground() && (int)unit->GetLevel() - (int)bot->GetLevel() > 4 && !unit->GetGUID().IsPlayer())
            continue;

        // if (needForQuestMap.find(unit->GetEntry()) == needForQuestMap.end())
        //     needForQuestMap[unit->GetEntry()] = needForQuest(unit);

        // if (!needForQuestMap[unit->GetEntry()])
        // {
        //     Creature* creature = dynamic_cast<Creature*>(unit);
        //     if ((urand(0, 100) < 60 || (context->GetValue<TravelTarget*>("travel target")->Get()->isWorking() &&
        //         context->GetValue<TravelTarget*>("travel target")->Get()->getDestination()->getName() != "GrindTravelDestination")))
        //     {
        //         continue;
        //     }
        // }

        if (Creature* creature = unit->ToCreature())
            if (CreatureTemplate const* CreatureTemplate = creature->GetCreatureTemplate())
                if (CreatureTemplate->rank > CREATURE_ELITE_NORMAL && !AI_VALUE(bool, "can fight elite"))
                    continue;

        if (!bot->IsWithinLOSInMap(unit))
        {
            continue;
        }

        if (group)
        {
            Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
            for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
            {
                Player* member = ObjectAccessor::FindPlayer(itr->guid);
                if (!member || !member->IsAlive())
                    continue;

                float d = member->GetDistance(unit);
                if (!result || d < distance)
                {
                    distance = d;
                    result = unit;
                }
            }
        }
        else
        {
            float newdistance = bot->GetDistance(unit);
            if (!result || (newdistance < distance))
            {
                distance = newdistance;
                result = unit;
            }
        }
    }

    return result;
}

bool GrindTargetValue::needForQuest(Unit* target)
{
    bool justCheck = (bot->GetGUID() == target->GetGUID());

    QuestStatusMap& questMap = bot->getQuestStatusMap();
    for (auto const& quest : questMap)
    {
        Quest const* questTemplate = sObjectMgr->GetQuestTemplate(quest.first);
        if (!questTemplate)
            continue;

        uint32 questId = questTemplate->GetQuestId();
        if (!questId)
            continue;

        QuestStatus status = bot->GetQuestStatus(questId);

        if (status == QUEST_STATUS_COMPLETE && !bot->GetQuestRewardStatus(questId))
        {
            if (!justCheck && !target->hasInvolvedQuest(questId))
                continue;

            return true;
        }
        else if (status == QUEST_STATUS_INCOMPLETE)
        {
            if (questTemplate->GetQuestLevel() > bot->GetLevel() + 5)
                continue;

            for (QuestObjective const& objective : questTemplate->GetObjectives())
            {
                if (objective.Amount <= 0)
                    continue;

                uint32 current = bot->GetQuestObjectiveCounter(objective.ID);
                if (current >= uint32(objective.Amount))
                    continue;

                switch (objective.Type)
                {
                    case QUEST_OBJECTIVE_MONSTER:
                        if (objective.ObjectID && (justCheck || target->GetEntry() == objective.ObjectID))
                            return true;
                        break;
                    case QUEST_OBJECTIVE_ITEM:
                    case QUEST_OBJECTIVE_GAMEOBJECT:
                        if (justCheck)
                            return true;
                        break;
                    default:
                        break;
                }
            }

            if (!justCheck)
            {
                if (CreatureTemplate const* data = sObjectMgr->GetCreatureTemplate(target->GetEntry()))
                {
                    if (uint32 lootId = data->lootid)
                    {
                        if (LootTemplates_Creature.HaveQuestLootForPlayer(lootId, bot))
                            return true;
                    }
                }
            }
        }
    }

    return false;
}

uint32 GrindTargetValue::GetTargetingPlayerCount(Unit* unit)
{
    Group* group = bot->GetGroup();
    if (!group)
        return 0;

    uint32 count = 0;
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        Player* member = ObjectAccessor::FindPlayer(itr->guid);
        if (!member || !member->IsAlive() || member == bot)
            continue;

        PlayerbotAI* botAI = GET_PLAYERBOT_AI(member);
        if ((botAI && *botAI->GetAiObjectContext()->GetValue<Unit*>("current target") == unit) ||
            (!botAI && member->GetTarget() == unit->GetGUID()))
            ++count;
    }

    return count;
}
