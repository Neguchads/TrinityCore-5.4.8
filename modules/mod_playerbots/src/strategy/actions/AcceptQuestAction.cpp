#include "AcceptQuestAction.h"

#include <cctype>

#include "Event.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Playerbots.h"
#include "QuestDef.h"

namespace
{
uint32 ExtractQuestId(std::string const& text)
{
    std::string digits;
    for (char c : text)
    {
        if (std::isdigit(static_cast<unsigned char>(c)))
            digits.push_back(c);
        else if (!digits.empty())
            break;
    }

    if (digits.empty())
        return 0;

    return static_cast<uint32>(std::stoul(digits));
}

uint32 ExtractQuestIdFromConfirmPacket(WorldPacket const& packet)
{
    if (packet.size() < sizeof(uint32))
        return 0;

    WorldPacket copy(packet);
    copy.rpos(copy.size() - sizeof(uint32));
    return copy.read<uint32>();
}
}

bool AcceptQuestAction::Execute(Event event)
{
    uint32 questId = 0;
    if (!event.getPacket().empty())
    {
        WorldPacket p(event.getPacket());
        p.rpos(0);
        questId = p.read<uint32>();
    }
    else if (!event.getParam().empty())
        questId = ExtractQuestId(event.getParam());

    if (!questId)
        return ProcessNearbyQuestGivers();

    return ProcessNearbyQuestGivers(questId);
}

bool AcceptAllQuestsAction::Execute(Event /*event*/)
{
    return ProcessNearbyQuestGivers();
}

bool TalkToQuestGiverAction::Execute(Event /*event*/)
{
    return ProcessNearbyQuestGivers();
}

bool TurnInQueryQuestAction::Execute(Event event)
{
    return TalkToQuestGiverAction::Execute(event);
}

bool AcceptQuestShareAction::Execute(Event event)
{
    if (event.getPacket().empty())
        return false;

    WorldPacket p(event.getPacket());
    p.rpos(0);

    uint32 questId = p.read<uint32>();
    if (!questId)
        return false;

    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    if (!quest)
        return false;

    if (bot->IsActiveQuest(questId) || bot->GetQuestStatus(questId) == QUEST_STATUS_REWARDED)
        return false;

    if (!bot->CanTakeQuest(quest, false) || !bot->CanAddQuest(quest, false))
        return false;

    bot->AddQuest(quest, GetMaster());
    if (quest->GetSrcSpell() > 0)
        bot->CastSpell(bot, quest->GetSrcSpell(), true);

    if (bot->CanCompleteQuest(questId))
        bot->CompleteQuest(questId);

    return true;
}

bool ConfirmQuestAction::Execute(Event event)
{
    uint32 questId = 0;

    if (!event.getPacket().empty())
        questId = ExtractQuestIdFromConfirmPacket(event.getPacket());
    else if (!event.getParam().empty())
        questId = ExtractQuestId(event.getParam());

    if (!questId)
        return false;

    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    if (!quest)
        return false;

    if (bot->IsActiveQuest(questId) || bot->GetQuestStatus(questId) == QUEST_STATUS_REWARDED)
        return false;

    WorldPacket confirmPacket(CMSG_QUEST_CONFIRM_ACCEPT, 4);
    confirmPacket << questId;
    confirmPacket.rpos(0);
    bot->GetSession()->HandleQuestConfirmAccept(confirmPacket);

    return bot->IsActiveQuest(questId);
}
