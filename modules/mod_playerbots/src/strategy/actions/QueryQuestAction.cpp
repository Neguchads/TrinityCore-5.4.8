#include "QueryQuestAction.h"

#include <algorithm>
#include <cmath>

#include "ChatHelper.h"
#include "Event.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Playerbots.h"
#include "QuestDef.h"
#include "TravelMgr.h"

void QueryQuestAction::TellObjective(std::string const& name, uint32 available, uint32 required)
{
    std::ostringstream out;
    out << name << " " << available << "/" << required;
    botAI->TellMaster(out);
}

bool QueryQuestAction::Execute(Event event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    if (!requester)
        return false;

    Player* bot = botAI->GetBot();
    WorldPosition botPos(bot);
    std::string text = event.getParam();
    bool travel = false;

    size_t travelPos = text.find("travel");
    if (travelPos != std::string::npos)
    {
        travel = true;
        size_t erasePos = text.find(" travel");
        if (erasePos != std::string::npos)
            text.erase(erasePos, 7);
    }

    PlayerbotChatHandler ch(bot);
    uint32 questId = ch.extractQuestId(text);
    if (!questId)
    {
        for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
        {
            uint32 logQuest = bot->GetQuestSlotQuestId(slot);
            Quest const* quest = sObjectMgr->GetQuestTemplate(logQuest);
            if (!quest)
                continue;

            if (text.find(quest->GetLogTitle()) != std::string::npos)
            {
                questId = quest->GetQuestId();
                break;
            }
        }
    }

    for (uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        if (questId != bot->GetQuestSlotQuestId(slot))
            continue;

        std::ostringstream out;
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            return false;

        out << "--- " << ChatHelper::FormatQuest(quest) << " ";

        if (bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE)
        {
            out << "|c0000FF00completed|r ---";
            botAI->TellMaster(out);
        }
        else
        {
            out << "|c00FF0000not completed|r ---";
            botAI->TellMaster(out);
            TellObjectives(questId);
        }

        if (travel)
        {
            uint32 limit = 0;
            std::vector<TravelDestination*> allDestinations =
                TravelMgr::instance().getQuestTravelDestinations(bot, questId, true, true, -1);

            std::sort(allDestinations.begin(), allDestinations.end(),
                      [&botPos](TravelDestination* i, TravelDestination* j)
                      {
                          return i->distanceTo(const_cast<WorldPosition*>(&botPos)) <
                                 j->distanceTo(const_cast<WorldPosition*>(&botPos));
                      });

            for (auto dest : allDestinations)
            {
                if (limit > 50)
                    continue;

                std::ostringstream line;
                uint32 tpoints = dest->getPoints(true).size();
                uint32 apoints = dest->getPoints().size();

                line << std::round(dest->distanceTo(&botPos));
                line << " to " << dest->getTitle();
                line << " " << apoints;

                if (apoints < tpoints)
                    line << "/" << tpoints;

                line << " points.";

                if (!dest->isActive(bot))
                    line << " not active";

                botAI->TellMaster(line);
                limit++;
            }
        }

        return true;
    }

    return false;
}

void QueryQuestAction::TellObjectives(uint32 questId)
{
    Quest const* questTemplate = sObjectMgr->GetQuestTemplate(questId);
    if (!questTemplate)
    {
        botAI->TellMaster("Quest template not found.");
        return;
    }

    for (QuestObjective const& obj : questTemplate->GetObjectives())
    {
        if (!obj.Description.empty())
            botAI->TellMaster(obj.Description);

        if (obj.StorageIndex < 0 || obj.StorageIndex >= QUEST_OBJECTIVES_COUNT)
            continue;

        if (obj.Type == QUEST_OBJECTIVE_ITEM)
        {
            uint32 required = obj.Amount;
            uint32 available = bot->GetQuestObjectiveCounter(obj.ID);
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(obj.ObjectID);
            if (proto)
                TellObjective(ChatHelper::FormatItem(proto), available, required);
        }
        else if (obj.Type == QUEST_OBJECTIVE_GAMEOBJECT)
        {
            uint32 required = obj.Amount;
            uint32 available = bot->GetQuestObjectiveCounter(obj.ID);
            if (GameObjectTemplate const* info = sObjectMgr->GetGameObjectTemplate(obj.ObjectID))
                TellObjective(info->name, available, required);
        }
        else if (obj.Type == QUEST_OBJECTIVE_MONSTER || obj.Type == QUEST_OBJECTIVE_TALKTO)
        {
            uint32 required = obj.Amount;
            uint32 available = bot->GetQuestObjectiveCounter(obj.ID);
            if (CreatureTemplate const* info = sObjectMgr->GetCreatureTemplate(obj.ObjectID))
                TellObjective(info->Name, available, required);
        }
    }
}
