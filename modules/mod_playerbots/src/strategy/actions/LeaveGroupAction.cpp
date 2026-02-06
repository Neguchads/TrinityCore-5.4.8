

#include "LeaveGroupAction.h"

#include "Event.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "ServerFacade.h"

bool LeaveGroupAction::Leave(Player* player)
{
    if (player && !GET_PLAYERBOT_AI(player))
        return false;

    bool aiMaster = GET_PLAYERBOT_AI(botAI->GetMaster()) != nullptr;

    if (bot->GetGroup())
    {
        //bot->GetGroup()->Disband();
        bot->RemoveFromGroup();
    }

    bool randomBot = sRandomPlayerbotMgr->IsRandomBot(bot);
    if (randomBot)
    {
        GET_PLAYERBOT_AI(bot)->SetMaster(nullptr);
    }

    if (!aiMaster)
        botAI->ResetStrategies();

    botAI->Reset();

    return true;
}

bool LeaveGroupAction::Execute(Event event)
{
    Player* master = event.getOwner();
    return Leave(master);
}

bool UninviteAction::Execute(Event event)
{
    WorldPacket& p = event.getPacket();
    if (p.GetOpcode() == CMSG_GROUP_DISBAND)
    {
        return Leave(bot);
    }

    if (p.GetOpcode() == CMSG_GROUP_UNINVITE_GUID)
    {
        p.rpos(1);
        uint32 guid_low;
        p >> guid_low;

        if (bot->GetGUID().GetCounter() == guid_low)
            return Leave(bot);
    }

    //botAI->Reset();

    return false;
}

bool LeaveFarAwayAction::Execute(Event event)
{
    return Leave(event.getOwner());
}

bool LeaveFarAwayAction::isUseful()
{
    if (bot->InBattleground() || bot->InBattlegroundQueue())
        return false;

    if (!bot->GetGroup())
        return false;

    if (botAI->HasRealPlayerMaster())
        return false;

    Player* groupLeader = botAI->GetGroupMaster();
    if (!groupLeader || groupLeader == bot)
        return false;

    if (bot->GetMapId() != groupLeader->GetMapId())
        return true;

    float distance = sServerFacade->GetDistance2d(bot, groupLeader);
    return distance >= 2.0f * sPlayerbotAIConfig->rpgDistance;
}
