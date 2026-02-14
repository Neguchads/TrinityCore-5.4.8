#include "AcceptInvitationAction.h"

#include "Event.h"
#include "Group.h"
#include "ObjectAccessor.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "PlayerbotSecurity.h"
#include "WorldPacket.h"

AcceptInvitationAction::AcceptInvitationAction(PlayerbotAI* botAI, const std::string name)
    : Action(botAI, name)
{
}

bool AcceptInvitationAction::Execute(Event event)
{
    Group* grp = bot->GetGroupInvite();
    if (!grp)
        return false;

    Player* inviter = event.getOwner();
    if (!inviter)
        inviter = ObjectAccessor::FindPlayer(grp->GetLeaderGUID());

    if (grp->GetLeaderGUID() == bot->GetGUID())
        return false;

    if (grp->IsFull())
        return false;

    Player* leader = ObjectAccessor::FindPlayer(grp->GetLeaderGUID());
    if (!grp->IsCreated())
    {
        if (!leader)
        {
            grp->RemoveAllInvites();
            return false;
        }

        grp->RemoveInvite(leader);
        grp->Create(leader);
    }

    grp->RemoveInvite(bot);
    if (!grp->AddMember(bot))
        return false;

    grp->BroadcastGroupUpdate();

    if (!botAI->GetSecurity()->CheckLevelFor(PlayerbotSecurityLevel::PLAYERBOT_SECURITY_INVITE, false, inviter))
    {
        WorldPacket data(SMSG_GROUP_DECLINE, 10);
        data << bot->GetName();
        inviter->SendDirectMessage(&data);
        bot->UninviteFromGroup();
        return false;
    }

    if (sRandomPlayerbotMgr->IsRandomBot(bot) && inviter)
        botAI->SetMaster(inviter);

    botAI->ResetStrategies();
    botAI->ChangeStrategy("+follow,-lfg,-bg", BOT_STATE_NON_COMBAT);
    botAI->Reset();

    if (inviter && bot->GetDistance(inviter) > sPlayerbotAIConfig->sightDistance)
    {
        //Teleport(inviter, bot);
    }
    return true;
}
