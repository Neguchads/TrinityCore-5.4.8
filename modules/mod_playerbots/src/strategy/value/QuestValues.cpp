#include "QuestValues.h"

#include "Creature.h"
#include "GameObject.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "Playerbots.h"
#include "QuestDef.h"
#include "SharedValueContext.h"

entryQuestRelationMap EntryQuestRelationMapValue::Calculate()
{
    entryQuestRelationMap rMap;

    for (auto relation : *sObjectMgr->GetCreatureQuestRelationMapHACK())
        rMap[relation.first][relation.second] |= static_cast<uint32>(QuestRelationFlag::questGiver);

    for (auto relation : *sObjectMgr->GetCreatureQuestInvolvedRelationMapHACK())
        rMap[relation.first][relation.second] |= static_cast<uint32>(QuestRelationFlag::questTaker);

    for (auto relation : *sObjectMgr->GetGOQuestRelationMapHACK())
        rMap[-static_cast<int32>(relation.first)][relation.second] |= static_cast<uint32>(QuestRelationFlag::questGiver);

    for (auto relation : *sObjectMgr->GetGOQuestInvolvedRelationMapHACK())
        rMap[-static_cast<int32>(relation.first)][relation.second] |= static_cast<uint32>(QuestRelationFlag::questTaker);

    ObjectMgr::QuestMap const& questMap = sObjectMgr->GetQuestTemplates();
    for (auto& questItr : questMap)
    {
        uint32 questId = questItr.first;
        Quest* quest = questItr.second;

        for (QuestObjective const& obj : quest->GetObjectives())
        {
            if (obj.StorageIndex < 0 || obj.StorageIndex >= QUEST_OBJECTIVES_COUNT)
                continue;

            uint32 relationFlag = 1u << obj.StorageIndex;

            if (obj.Type == QUEST_OBJECTIVE_MONSTER || obj.Type == QUEST_OBJECTIVE_TALKTO)
            {
                if (obj.ObjectID)
                    rMap[static_cast<int32>(obj.ObjectID)][questId] |= relationFlag;
            }
            else if (obj.Type == QUEST_OBJECTIVE_GAMEOBJECT)
            {
                if (obj.ObjectID)
                    rMap[-static_cast<int32>(obj.ObjectID)][questId] |= relationFlag;
            }
            else if (obj.Type == QUEST_OBJECTIVE_ITEM)
            {
                for (auto const& entry : GAI_VALUE2(std::vector<int32>, "item drop list", obj.ObjectID))
                    rMap[entry][questId] |= relationFlag;
            }
        }
    }

    return rMap;
}

void FindQuestObjectData::GetObjectiveEntries()
{
    relationMap = GAI_VALUE(entryQuestRelationMap, "entry quest relation map");
}

void FindQuestObjectData::operator()(CreatureData const& creData)
{
    uint32 entry = creData.id;

    for (auto& relation : relationMap[static_cast<int32>(entry)])
    {
        uint32 questId = relation.first;
        uint32 flag = relation.second;
        data[questId][flag][static_cast<int32>(entry)].push_back(GuidPosition(creData));
    }
}

void FindQuestObjectData::operator()(GameObjectData const& goData)
{
    int32 entry = -static_cast<int32>(goData.id);

    for (auto& relation : relationMap[entry])
    {
        uint32 questId = relation.first;
        uint32 flag = relation.second;
        data[questId][flag][entry].push_back(GuidPosition(goData));
    }
}

questGuidpMap QuestGuidpMapValue::Calculate()
{
    FindQuestObjectData worker;

    for (auto const& itr : sObjectMgr->GetAllCreatureData())
        worker(itr.second);
    for (auto const& itr : sObjectMgr->GetAllGOData())
        worker(itr.second);

    return worker.GetResult();
}

questGiverMap QuestGiversValue::Calculate()
{
    uint32 level = 0;
    std::string const q = getQualifier();
    bool hasQualifier = !q.empty();

    if (hasQualifier)
        level = stoi(q);

    questGuidpMap questMap = GAI_VALUE(questGuidpMap, "quest guidp map");
    questGiverMap guidps;

    for (auto& qPair : questMap)
    {
        for (auto& entry : qPair.second[static_cast<uint32>(QuestRelationFlag::questGiver)])
        {
            for (auto& guidp : entry.second)
            {
                uint32 questId = qPair.first;
                if (hasQualifier)
                {
                    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
                    if (quest && (level < quest->GetMinLevel() || static_cast<int32>(level) > quest->GetQuestLevel() + 10))
                        continue;
                }

                guidps[questId].push_back(guidp);
            }
        }
    }

    return guidps;
}

std::vector<GuidPosition> ActiveQuestGiversValue::Calculate()
{
    questGiverMap qGivers = GAI_VALUE2(questGiverMap, "quest givers", bot->GetLevel());
    std::vector<GuidPosition> retQuestGivers;

    for (auto& qGiver : qGivers)
    {
        uint32 questId = qGiver.first;
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        if (!bot->CanTakeQuest(quest, false))
            continue;

        QuestStatus status = bot->GetQuestStatus(questId);
        if (status != QUEST_STATUS_NONE)
            continue;

        for (auto& guidp : qGiver.second)
        {
            CreatureTemplate const* creatureTemplate = guidp.GetCreatureTemplate();
            if (creatureTemplate)
            {
                FactionTemplateEntry const* botFaction = bot->GetFactionTemplateEntry();
                FactionTemplateEntry const* targetFaction = sFactionTemplateStore.LookupEntry(creatureTemplate->faction);
                if (botFaction && targetFaction && !botFaction->IsFriendlyTo(*targetFaction))
                    continue;
            }

            if (guidp.isDead())
                continue;

            retQuestGivers.push_back(guidp);
        }
    }

    return retQuestGivers;
}

std::vector<GuidPosition> ActiveQuestTakersValue::Calculate()
{
    questGuidpMap questMap = GAI_VALUE(questGuidpMap, "quest guidp map");
    std::vector<GuidPosition> retQuestTakers;

    QuestStatusMap& questStatusMap = bot->getQuestStatusMap();
    for (auto& questStatus : questStatusMap)
    {
        uint32 questId = questStatus.first;
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        QuestStatus status = questStatus.second.Status;
        if ((status != QUEST_STATUS_COMPLETE || bot->GetQuestRewardStatus(questId)) &&
            (!quest->IsTurnIn() || !bot->CanTakeQuest(quest, false)))
            continue;

        auto q = questMap.find(questId);
        if (q == questMap.end())
            continue;

        auto qt = q->second.find(static_cast<uint32>(QuestRelationFlag::questTaker));
        if (qt == q->second.end())
            continue;

        for (auto& entry : qt->second)
        {
            if (entry.first > 0)
            {
                if (CreatureTemplate const* info = sObjectMgr->GetCreatureTemplate(entry.first))
                {
                    FactionTemplateEntry const* botFaction = bot->GetFactionTemplateEntry();
                    FactionTemplateEntry const* targetFaction = sFactionTemplateStore.LookupEntry(info->faction);
                    if (botFaction && targetFaction && !botFaction->IsFriendlyTo(*targetFaction))
                        continue;
                }
            }

            for (auto& guidp : entry.second)
            {
                if (guidp.isDead())
                    continue;

                retQuestTakers.push_back(guidp);
            }
        }
    }

    return retQuestTakers;
}

std::vector<GuidPosition> ActiveQuestObjectivesValue::Calculate()
{
    questGuidpMap questMap = GAI_VALUE(questGuidpMap, "quest guidp map");
    std::vector<GuidPosition> retQuestObjectives;

    QuestStatusMap& questStatusMap = bot->getQuestStatusMap();
    for (auto& questStatus : questStatusMap)
    {
        uint32 questId = questStatus.first;
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        QuestStatusData statusData = questStatus.second;
        if (statusData.Status != QUEST_STATUS_INCOMPLETE)
            continue;

        for (QuestObjective const& obj : quest->GetObjectives())
        {
            if (obj.StorageIndex < 0 || obj.StorageIndex >= QUEST_OBJECTIVES_COUNT)
                continue;

            if (obj.Type != QUEST_OBJECTIVE_ITEM && obj.Type != QUEST_OBJECTIVE_MONSTER &&
                obj.Type != QUEST_OBJECTIVE_GAMEOBJECT && obj.Type != QUEST_OBJECTIVE_TALKTO)
                continue;

            uint32 reqCount = obj.Amount;
            uint32 hasCount = bot->GetQuestObjectiveCounter(obj.ID);
            if (!reqCount || hasCount >= reqCount)
                continue;

            auto q = questMap.find(questId);
            if (q == questMap.end())
                continue;

            auto qt = q->second.find(1u << obj.StorageIndex);
            if (qt == q->second.end())
                continue;

            for (auto& entry : qt->second)
            {
                for (auto& guidp : entry.second)
                {
                    if (guidp.isDead())
                        continue;

                    retQuestObjectives.push_back(guidp);
                }
            }
        }
    }

    return retQuestObjectives;
}

uint8 FreeQuestLogSlotValue::Calculate()
{
    uint8 numQuest = 0;
    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 questId = bot->GetQuestSlotQuestId(slot);
        if (!questId)
            continue;

        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        numQuest++;
    }

    return MAX_QUEST_LOG_SIZE - numQuest;
}

uint32 DialogStatusValue::getDialogStatus(Player* bot, int32 questgiver, uint32 questId)
{
    uint32 dialogStatus = DIALOG_STATUS_NONE;
    QuestRelationResult rbounds;
    QuestRelationResult irbounds;

    if (questgiver > 0)
    {
        rbounds = sObjectMgr->GetCreatureQuestRelations(questgiver);
        irbounds = sObjectMgr->GetCreatureQuestInvolvedRelations(questgiver);
    }
    else
    {
        rbounds = sObjectMgr->GetGOQuestRelations(-questgiver);
        irbounds = sObjectMgr->GetGOQuestInvolvedRelations(-questgiver);
    }

    for (uint32 relQuestId : irbounds)
    {
        if (questId && relQuestId != questId)
            continue;

        Quest const* quest = sObjectMgr->GetQuestTemplate(relQuestId);
        if (!quest)
            continue;

        uint32 dialogStatusNew = DIALOG_STATUS_NONE;
        QuestStatus status = bot->GetQuestStatus(relQuestId);

        if ((status == QUEST_STATUS_COMPLETE && !bot->GetQuestRewardStatus(relQuestId)) ||
            (quest->IsTurnIn() && bot->CanTakeQuest(quest, false)))
        {
            if (quest->IsTurnIn() && quest->IsRepeatable())
                dialogStatusNew = DIALOG_STATUS_REWARD_REP;
            else
                dialogStatusNew = DIALOG_STATUS_REWARD2;
        }
        else if (status == QUEST_STATUS_INCOMPLETE)
            dialogStatusNew = DIALOG_STATUS_INCOMPLETE;

        if (dialogStatusNew > dialogStatus)
            dialogStatus = dialogStatusNew;
    }

    for (uint32 relQuestId : rbounds)
    {
        if (questId && relQuestId != questId)
            continue;

        Quest const* quest = sObjectMgr->GetQuestTemplate(relQuestId);
        if (!quest)
            continue;

        uint32 dialogStatusNew = DIALOG_STATUS_NONE;
        QuestStatus status = bot->GetQuestStatus(relQuestId);

        if (status == QUEST_STATUS_NONE)
        {
            if (bot->CanSeeStartQuest(quest))
            {
                if (bot->SatisfyQuestLevel(quest, false))
                {
                    int32 lowLevelDiff = sWorld->getIntConfig(CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF);
                    if (quest->IsTurnIn() || (quest->IsRepeatable() &&
                        bot->getQuestStatusMap()[relQuestId].Status == QUEST_STATUS_REWARDED))
                    {
                        dialogStatusNew = DIALOG_STATUS_REWARD_REP;
                    }
                    else if (lowLevelDiff < 0 || bot->GetLevel() <= bot->GetQuestLevel(quest) + uint32(lowLevelDiff))
                        dialogStatusNew = DIALOG_STATUS_AVAILABLE;
                    else
                        dialogStatusNew = DIALOG_STATUS_LOW_LEVEL_AVAILABLE;
                }
                else
                    dialogStatusNew = DIALOG_STATUS_UNAVAILABLE;
            }
        }

        if (dialogStatusNew > dialogStatus)
            dialogStatus = dialogStatusNew;
    }

    return dialogStatus;
}

uint32 DialogStatusValue::Calculate()
{
    return getDialogStatus(bot, stoi(getQualifier()));
}

uint32 DialogStatusQuestValue::Calculate()
{
    return getDialogStatus(bot, getMultiQualifier(getQualifier(), 0), getMultiQualifier(getQualifier(), 1));
}

bool CanAcceptQuestValue::Calculate()
{
    return AI_VALUE2(uint32, "dialog status", getQualifier()) == DIALOG_STATUS_AVAILABLE;
}

bool CanAcceptQuestLowLevelValue::Calculate()
{
    uint32 dialogStatus = AI_VALUE2(uint32, "dialog status", getQualifier());
    return dialogStatus == DIALOG_STATUS_LOW_LEVEL_AVAILABLE;
}

bool CanTurnInQuestValue::Calculate()
{
    uint32 dialogStatus = AI_VALUE2(uint32, "dialog status", getQualifier());
    return dialogStatus == DIALOG_STATUS_REWARD2 || dialogStatus == DIALOG_STATUS_REWARD ||
           dialogStatus == DIALOG_STATUS_REWARD_REP;
}
