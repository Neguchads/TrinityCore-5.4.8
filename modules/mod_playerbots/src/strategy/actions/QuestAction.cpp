#include "QuestAction.h"

#include "ChatHelper.h"
#include "GameObject.h"
#include "GossipDef.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Playerbots.h"
#include "QuestDef.h"
#include "Unit.h"

bool QuestAction::ProcessNearbyQuestGivers(uint32 questId)
{
    bool handled = false;

    if (Player* master = GetMaster())
    {
        ObjectGuid masterTarget = master->GetTarget();
        if (masterTarget)
            handled |= ProcessQuestGiver(masterTarget, questId);
    }

    ObjectGuid botTarget = bot->GetTarget();
    if (botTarget)
        handled |= ProcessQuestGiver(botTarget, questId);

    GuidVector npcs = AI_VALUE(GuidVector, "nearest npcs");
    for (ObjectGuid const& guid : npcs)
        handled |= ProcessQuestGiver(guid, questId);

    GuidVector gos = AI_VALUE(GuidVector, "nearest game objects no los");
    for (ObjectGuid const& guid : gos)
        handled |= ProcessQuestGiver(guid, questId);

    return handled;
}

bool QuestAction::ProcessQuestGiver(ObjectGuid guid, uint32 questId)
{
    if (!guid)
        return false;

    if (Unit* unit = botAI->GetUnit(guid))
        return ProcessQuestGiver(unit, questId);

    if (GameObject* go = botAI->GetGameObject(guid))
        return ProcessQuestGiver(go, questId);

    return false;
}

bool QuestAction::ProcessQuestGiver(WorldObject* questGiver, uint32 questId)
{
    if (!questGiver)
        return false;

    if (bot->GetDistance(questGiver) > INTERACTION_DISTANCE)
        return false;

    if (!bot->HasInArc(CAST_ANGLE_IN_FRONT, questGiver, sPlayerbotAIConfig->sightDistance))
        bot->SetFacingToObject(questGiver);

    Object* questObject = static_cast<Object*>(questGiver);
    ObjectGuid guid = questGiver->GetGUID();
    bot->SetTarget(guid);
    bot->PrepareQuestMenu(guid);

    QuestMenu& questMenu = bot->PlayerTalkClass->GetQuestMenu();
    bool handled = false;

    for (uint32 i = 0; i < questMenu.GetMenuItemCount(); ++i)
    {
        QuestMenuItem const& item = questMenu.GetItem(i);
        Quest const* quest = sObjectMgr->GetQuestTemplate(item.QuestId);
        if (!quest)
            continue;

        if (questId && quest->GetQuestId() != questId)
            continue;

        uint32 status = bot->GetQuestStatus(quest->GetQuestId());
        if (status == QUEST_STATUS_NONE)
            handled |= AcceptQuestFrom(questObject, quest);

        if (status == QUEST_STATUS_COMPLETE || bot->CanCompleteQuest(quest->GetQuestId()))
            handled |= TryCompleteQuest(quest, questObject);
    }

    return handled;
}

bool QuestAction::AcceptQuestFrom(Object* questGiver, Quest const* quest)
{
    if (!questGiver || !quest)
        return false;

    if (!bot->CanTakeQuest(quest, false))
        return false;

    if (!bot->CanAddQuest(quest, false))
        return false;

    bot->AddQuest(quest, questGiver);
    if (quest->GetSrcSpell() > 0)
        bot->CastSpell(bot, quest->GetSrcSpell(), true);

    if (bot->CanCompleteQuest(quest->GetQuestId()))
        bot->CompleteQuest(quest->GetQuestId());

    return bot->GetQuestStatus(quest->GetQuestId()) != QUEST_STATUS_NONE;
}

bool QuestAction::TryCompleteQuest(Quest const* quest, Object* questGiver)
{
    if (!quest)
        return false;

    if (bot->CanCompleteQuest(quest->GetQuestId()))
        bot->CompleteQuest(quest->GetQuestId());

    if (bot->GetQuestStatus(quest->GetQuestId()) == QUEST_STATUS_COMPLETE)
        return TryTurnInQuest(quest, questGiver);

    return false;
}

bool QuestAction::TryTurnInQuest(Quest const* quest, Object* questGiver)
{
    if (!quest || !questGiver)
        return false;

    uint32 rewardChoice = 0;
    if (quest->GetRewChoiceItemsCount() > 0)
    {
        bool foundReward = false;
        for (uint32 i = 0; i < quest->GetRewChoiceItemsCount(); ++i)
        {
            if (bot->CanRewardQuest(quest, i, false))
            {
                rewardChoice = i;
                foundReward = true;
                break;
            }
        }

        if (!foundReward)
            return false;
    }
    else if (!bot->CanRewardQuest(quest, false))
        return false;

    bot->RewardQuest(quest, rewardChoice, questGiver, false);
    return bot->GetQuestStatus(quest->GetQuestId()) == QUEST_STATUS_REWARDED;
}

bool QuestUpdateAddKillAction::Execute(Event event)
{
    WorldPacket p(event.getPacket());
    p.rpos(0);

    uint32 questId = 0;
    uint32 objectId = 0;
    uint32 available = 0;
    uint32 required = 0;
    uint8 objectiveType = 0;

    if (p.GetOpcode() == SMSG_QUESTUPDATE_ADD_CREDIT)
    {
        uint16 available16 = 0;
        uint16 required16 = 0;
        p >> available16;
        p >> objectiveType;
        p >> questId;
        p >> required16;
        p >> objectId;
        available = available16;
        required = required16;
    }
    else if (p.GetOpcode() == SMSG_QUESTUPDATE_ADD_CREDIT_SIMPLE)
    {
        p >> objectId;
        p >> questId;
        p >> objectiveType;
    }

    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    if (!quest)
        return false;

    if (objectiveType == QUEST_OBJECTIVE_ITEM || required == 0)
    {
        for (QuestObjective const& obj : quest->GetObjectives())
        {
            if (obj.ObjectID != objectId || obj.Type != objectiveType)
                continue;
            if (obj.StorageIndex < 0 || obj.StorageIndex >= QUEST_OBJECTIVES_COUNT)
                continue;

            required = obj.Amount;
            available = bot->GetQuestObjectiveCounter(obj.ID);
            break;
        }
    }

    std::string targetName;
    if (objectiveType == QUEST_OBJECTIVE_GAMEOBJECT)
    {
        if (GameObjectTemplate const* info = sObjectMgr->GetGameObjectTemplate(objectId))
            targetName = info->name;
    }
    else
    {
        if (CreatureTemplate const* info = sObjectMgr->GetCreatureTemplate(objectId))
            targetName = info->Name;
    }

    if (targetName.empty())
        targetName = "Objective";

    if (botAI->GetMaster())
    {
        std::ostringstream out;
        out << targetName;
        if (required > 0)
            out << " " << available << "/" << required;
        out << " " << ChatHelper::FormatQuest(quest);
        botAI->TellMasterNoFacing(out.str());
    }

    return true;
}

bool QuestUpdateFailedAction::Execute(Event event)
{
    WorldPacket p(event.getPacket());
    p.rpos(0);

    uint32 questId = 0;
    p >> questId;

    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    if (quest)
        botAI->TellMaster("Quest failed " + ChatHelper::FormatQuest(quest));
    else
        botAI->TellMaster("Quest failed " + std::to_string(questId));

    return true;
}

bool QuestUpdateFailedTimerAction::Execute(Event event)
{
    WorldPacket p(event.getPacket());
    p.rpos(0);

    uint32 questId = 0;
    p >> questId;

    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    if (quest)
        botAI->TellMaster("Quest failed timer " + ChatHelper::FormatQuest(quest));
    else
        botAI->TellMaster("Quest failed timer " + std::to_string(questId));

    return true;
}

bool QuestUpdateCompleteAction::Execute(Event event)
{
    WorldPacket p(event.getPacket());
    p.rpos(0);

    uint32 questId = 0;
    p >> questId;

    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    if (quest)
        botAI->TellMaster("Quest completed " + ChatHelper::FormatQuest(quest));
    else
        botAI->TellMaster("Quest completed " + std::to_string(questId));

    return true;
}

bool QuestItemPushResultAction::Execute(Event /*event*/)
{
    return false;
}
