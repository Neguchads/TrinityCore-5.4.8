

#ifndef _PLAYERBOT_WORLDPACKETTRIGGERCONTEXT_H
#define _PLAYERBOT_WORLDPACKETTRIGGERCONTEXT_H

#include "NamedObjectContext.h"
#include "WorldPacketTrigger.h"

class WorldPacketTriggerContext : public NamedObjectContext<Trigger>
{
public:
    WorldPacketTriggerContext()
    {
        creators["group invite"] = &WorldPacketTriggerContext::group_invite;
        creators["uninvite"] = &WorldPacketTriggerContext::uninvite;
        creators["uninvite guid"] = &WorldPacketTriggerContext::uninvite_guid;
        creators["group destroyed"] = &WorldPacketTriggerContext::group_destroyed;

        creators["release spirit"] = &WorldPacketTriggerContext::release_spirit;
        creators["revive from corpse"] = &WorldPacketTriggerContext::revive_from_corpse;
        creators["resurrect request"] = &WorldPacketTriggerContext::resurrect_request;

        creators["levelup"] = &WorldPacketTriggerContext::levelup;
        creators["duel requested"] = &WorldPacketTriggerContext::duel_requested;
        creators["bg status"] = &WorldPacketTriggerContext::bg_status;
        creators["lfg role check"] = &WorldPacketTriggerContext::lfg_role_check;
        creators["lfg proposal"] = &WorldPacketTriggerContext::lfg_proposal;
        creators["lfg teleport"] = &WorldPacketTriggerContext::lfg_teleport;
        creators["check mount state"] = &WorldPacketTriggerContext::check_mount_state;
    }

private:
    static Trigger* group_invite(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "group invite"); }
    static Trigger* uninvite(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "uninvite"); }
    static Trigger* uninvite_guid(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "uninvite guid"); }
    static Trigger* group_destroyed(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "group destroyed"); }

    static Trigger* release_spirit(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "release spirit"); }
    static Trigger* revive_from_corpse(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "revive from corpse"); }
    static Trigger* resurrect_request(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "resurrect request"); }

    static Trigger* levelup(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "levelup"); }
    static Trigger* duel_requested(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "duel requested"); }
    static Trigger* bg_status(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "bg status"); }
    static Trigger* lfg_role_check(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "lfg role check"); }
    static Trigger* lfg_proposal(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "lfg proposal"); }
    static Trigger* lfg_teleport(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "lfg teleport"); }
    static Trigger* check_mount_state(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "check mount state"); }
};

#endif
