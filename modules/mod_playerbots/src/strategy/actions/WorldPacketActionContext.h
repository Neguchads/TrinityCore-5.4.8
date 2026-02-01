

#ifndef _PLAYERBOT_WORLDPACKETACTIONCONTEXT_H
#define _PLAYERBOT_WORLDPACKETACTIONCONTEXT_H


#include "NamedObjectContext.h"

#include "AcceptDuelAction.h"
#include "AcceptResurrectAction.h"
#include "AcceptInvitationAction.h"
#include "BattleGroundJoinAction.h"
#include "ReleaseSpiritAction.h"
#include "ReviveFromCorpseAction.h"
#include "LeaveGroupAction.h"
#include "LfgActions.h"

class PlayerbotAI;

class WorldPacketActionContext : public NamedObjectContext<Action>
{
public:
    WorldPacketActionContext()
    {
        // party handlers
        creators["accept invitation"] = &WorldPacketActionContext::accept_invitation;
        creators["uninvite"] = &WorldPacketActionContext::uninvite;

        // death
        creators["revive from corpse"] = &WorldPacketActionContext::revive_from_corpse;
        creators["find corpse"] = &WorldPacketActionContext::find_corpse;
        creators["auto release"] = &WorldPacketActionContext::auto_release;
        creators["accept resurrect"] = &WorldPacketActionContext::accept_resurrect;

        creators["accept duel"] = &WorldPacketActionContext::accept_duel;
        creators["lfg accept"] = &WorldPacketActionContext::lfg_accept;
        creators["lfg role check"] = &WorldPacketActionContext::lfg_role_check;
        creators["lfg teleport"] = &WorldPacketActionContext::lfg_teleport;
        creators["bg status"] = &WorldPacketActionContext::bg_status;
        creators["bg strategy check"] = &WorldPacketActionContext::bg_strategy_check;
    }

private:
    static Action* accept_invitation(PlayerbotAI* botAI) { return new AcceptInvitationAction(botAI); }
    static Action* uninvite(PlayerbotAI* botAI) { return new UninviteAction(botAI); }

    static Action* find_corpse(PlayerbotAI* botAI) { return new FindCorpseAction(botAI); }
    static Action* auto_release(PlayerbotAI* botAI) { return new AutoReleaseSpiritAction(botAI); }
    static Action* revive_from_corpse(PlayerbotAI* botAI) { return new ReviveFromCorpseAction(botAI); }
    static Action* accept_resurrect(PlayerbotAI* botAI) { return new AcceptResurrectAction(botAI); }
    static Action* accept_duel(PlayerbotAI* botAI) { return new AcceptDuelAction(botAI); }
    static Action* lfg_accept(PlayerbotAI* botAI) { return new LfgAcceptAction(botAI); }
    static Action* lfg_role_check(PlayerbotAI* botAI) { return new LfgRoleCheckAction(botAI); }
    static Action* lfg_teleport(PlayerbotAI* botAI) { return new LfgTeleportAction(botAI); }
    static Action* bg_status(PlayerbotAI* botAI) { return new BGStatusAction(botAI); }
    static Action* bg_strategy_check(PlayerbotAI* botAI) { return new BGStrategyCheckAction(botAI); }
};

#endif
