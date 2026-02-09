#include "DropQuestAction.h"

#include <algorithm>
#include <random>

#include "ChatHelper.h"
#include "Event.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Playerbots.h"
#include "QuestDef.h"

bool DropQuestAction::Execute(Event event)
{
    std::string const link = event.getParam();

    Player* master = GetMaster();
    if (!master)
        return false;

    PlayerbotChatHandler handler(master);
    uint32 entry = handler.extractQuestId(link);

    // remove quest entries for 'entry' from quest log
    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 logQuest = bot->GetQuestSlotQuestId(slot);

        Quest const* quest = sObjectMgr->GetQuestTemplate(logQuest);
        if (!quest)
            continue;

        if (logQuest == entry || link.find(quest->GetLogTitle()) != std::string::npos)
        {
            bot->SetQuestSlot(slot, 0);
            bot->TakeQuestSourceItem(logQuest, false);
            entry = logQuest;
            break;
        }
    }

    if (!entry)
        return false;

    bot->RemoveRewardedQuest(entry);
    bot->RemoveActiveQuest(entry, false);

    Quest const* quest = sObjectMgr->GetQuestTemplate(entry);
    if (quest && (botAI->HasStrategy("debug quest", BOT_STATE_NON_COMBAT) || botAI->HasStrategy("debug rpg", BOT_STATE_COMBAT)))
    {
        TC_LOG_INFO("playerbots", "{} => Quest [ {} ] removed", bot->GetName(), quest->GetLogTitle());
        bot->Say("Quest [ " + ChatHelper::FormatQuest(quest) + " ] removed", LANG_UNIVERSAL);
    }

    botAI->TellMaster("Quest removed");
    return true;
}

bool CleanQuestLogAction::Execute(Event event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    if (!requester)
    {
        botAI->TellMaster("No event owner detected");
        return false;
    }

    uint8 botLevel = bot->GetLevel();
    uint8 numQuest = 0;
    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        if (bot->GetQuestSlotQuestId(slot))
            numQuest++;
    }

    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 questId = bot->GetQuestSlotQuestId(slot);
        if (!questId)
            continue;

        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        int32 questLevel = quest->GetQuestLevel();
        if (questLevel == -1)
            questLevel = botLevel;

        int32 trivialLevel = 5;
        if (botLevel >= 40)
            trivialLevel = 8;
        else if (botLevel >= 30)
            trivialLevel = 7;
        else if (botLevel >= 20)
            trivialLevel = 6;

        if ((botLevel - questLevel) > trivialLevel)
        {
            bot->SetQuestSlot(slot, 0);
            bot->TakeQuestSourceItem(questId, false);
            bot->SetQuestStatus(questId, QUEST_STATUS_NONE);
            bot->RemoveRewardedQuest(questId);
            numQuest--;

            if (botAI->HasStrategy("debug quest", BOT_STATE_NON_COMBAT) || botAI->HasStrategy("debug rpg", BOT_STATE_COMBAT))
            {
                TC_LOG_INFO("playerbots", "{} => Quest [ {} ] removed", bot->GetName(), quest->GetLogTitle());
                bot->Say("Quest [ " + ChatHelper::FormatQuest(quest) + " ] removed", LANG_UNIVERSAL);
            }
        }
    }

    return true;
}

void CleanQuestLogAction::DropQuestType(uint8& numQuest, uint8 wantNum, bool isGreen, bool hasProgress, bool isComplete)
{
    std::vector<uint8> slots;
    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
        slots.push_back(slot);

    if (wantNum < 100)
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(slots.begin(), slots.end(), g);
    }

    for (uint8 slot : slots)
    {
        uint32 questId = bot->GetQuestSlotQuestId(slot);
        if (!questId)
            continue;

        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        if (quest->GetAllowableClasses())
            continue;

        if (wantNum == 100)
            numQuest++;

        int32 lowLevelDiff = sWorld->getIntConfig(CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF);
        if (lowLevelDiff < 0 || bot->GetLevel() <= bot->GetQuestLevel(quest) + uint32(lowLevelDiff))
        {
            if (bot->GetLevel() + 5 > bot->GetQuestLevel(quest))
                if (!isGreen)
                    continue;
        }
        else
        {
            if (isGreen)
                continue;
        }

        if (HasProgress(bot, quest) && !hasProgress && bot->GetQuestStatus(questId) != QUEST_STATUS_FAILED)
            continue;

        if (bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE && !isComplete)
            continue;

        if (numQuest <= wantNum && bot->GetQuestStatus(questId) != QUEST_STATUS_FAILED)
            continue;

        bot->SetQuestSlot(slot, 0);
        bot->TakeQuestSourceItem(questId, false);
        bot->SetQuestStatus(questId, QUEST_STATUS_NONE);
        bot->RemoveRewardedQuest(questId);

        numQuest--;

        if (botAI->HasStrategy("debug quest", BOT_STATE_NON_COMBAT) || botAI->HasStrategy("debug rpg", BOT_STATE_COMBAT))
        {
            TC_LOG_INFO("playerbots", "{} => Quest [ {} ] removed", bot->GetName(), quest->GetLogTitle());
            bot->Say("Quest [ " + ChatHelper::FormatQuest(quest) + " ] removed", LANG_UNIVERSAL);
        }
        botAI->TellMaster("Quest removed " + ChatHelper::FormatQuest(quest));
    }
}

bool CleanQuestLogAction::HasProgress(Player* bot, Quest const* quest)
{
    uint32 questId = quest->GetQuestId();

    if (bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE)
        return true;

    for (QuestObjective const& obj : quest->GetObjectives())
    {
        if (!obj.Description.empty())
            return true;

        if (obj.StorageIndex < 0 || obj.StorageIndex >= QUEST_OBJECTIVES_COUNT)
            continue;

        if (obj.Type == QUEST_OBJECTIVE_ITEM)
        {
            uint32 required = obj.Amount;
            uint32 available = bot->GetQuestObjectiveCounter(obj.ID);
            if (available > 0 && required > 0)
                return true;
        }
        else if (obj.Type == QUEST_OBJECTIVE_MONSTER || obj.Type == QUEST_OBJECTIVE_GAMEOBJECT ||
                 obj.Type == QUEST_OBJECTIVE_TALKTO)
        {
            uint32 required = obj.Amount;
            uint32 available = bot->GetQuestObjectiveCounter(obj.ID);
            if (available > 0 && required > 0)
                return true;
        }
    }

    return false;
}
