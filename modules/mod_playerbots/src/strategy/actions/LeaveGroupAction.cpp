

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

    if (randomBot || !aiMaster)
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
        p.rpos(0);
        ObjectGuid guid;

        p.read_skip<uint8>();

        guid[6] = p.ReadBit();
        guid[4] = p.ReadBit();
        guid[3] = p.ReadBit();
        guid[2] = p.ReadBit();
        guid[0] = p.ReadBit();
        guid[1] = p.ReadBit();
        guid[7] = p.ReadBit();
        guid[5] = p.ReadBit();

        uint8 reasonLen = p.ReadBits(8);
        p.ReadString(reasonLen);

        p.ReadByteSeq(guid[5]);
        p.ReadByteSeq(guid[6]);
        p.ReadByteSeq(guid[1]);
        p.ReadByteSeq(guid[4]);
        p.ReadByteSeq(guid[3]);
        p.ReadByteSeq(guid[2]);
        p.ReadByteSeq(guid[7]);
        p.ReadByteSeq(guid[0]);

        if (bot->GetGUID() == guid)
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
