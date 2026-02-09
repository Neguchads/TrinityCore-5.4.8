#include "ShareQuestAction.h"

#include "Event.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Playerbots.h"
#include "QuestDef.h"

bool ShareQuestAction::Execute(Event event)
{
    std::string const link = event.getParam();
    if (!GetMaster())
        return false;

    PlayerbotChatHandler handler(GetMaster());
    uint32 entry = handler.extractQuestId(link);
    if (!entry)
        return false;

    Quest const* quest = sObjectMgr->GetQuestTemplate(entry);
    if (!quest)
        return false;

    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 logQuest = bot->GetQuestSlotQuestId(slot);
        if (logQuest == entry)
        {
            WorldPacket p(CMSG_PUSHQUESTTOPARTY, 4);
            p << entry;
            bot->GetSession()->HandlePushQuestToParty(p);
            botAI->TellMaster("Quest shared");
            return true;
        }
    }

    return false;
}

bool AutoShareQuestAction::Execute(Event event)
{
    if (!bot->GetGroup())
        return false;

    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    bool shared = false;

    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 logQuest = bot->GetQuestSlotQuestId(slot);
        Quest const* quest = sObjectMgr->GetQuestTemplate(logQuest);
        if (!quest)
            continue;

        bool partyNeedsQuest = false;

        for (GroupReference* itr = bot->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* player = itr->GetSource();
            if (!player || player == bot || !player->IsInWorld() || !botAI->IsSafe(player))
                continue;

            if (bot->GetDistance(player) > 10.0f)
                continue;

            if (!player->SatisfyQuestStatus(quest, false))
                continue;

            if (player->GetQuestStatus(logQuest) == QUEST_STATUS_COMPLETE)
                continue;

            if (!player->CanTakeQuest(quest, false))
                continue;

            if (!player->SatisfyQuestLog(false))
                continue;

            if (player->GetDivider())
                continue;

            if (auto ai = GET_PLAYERBOT_AI(player))
            {
                if (PAI_VALUE(uint8, "free quest log slots") < 15 || !urand(0, 5))
                {
                    WorldPacket packet(CMSG_PUSHQUESTTOPARTY, 4);
                    packet << logQuest;
                    ai->HandleMasterIncomingPacket(packet);
                }
            }
            else
            {
                partyNeedsQuest = true;
            }
        }

        if (!partyNeedsQuest)
            continue;

        WorldPacket p(CMSG_PUSHQUESTTOPARTY, 4);
        p << logQuest;
        bot->GetSession()->HandlePushQuestToParty(p);
        botAI->TellMaster("Quest shared");
        shared = true;
    }

    return shared;
}

bool AutoShareQuestAction::isUseful()
{
    return bot->GetGroup() && !botAI->HasActivePlayerMaster();
}
