

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
        creators["complete quest"] = &WorldPacketTriggerContext::complete_quest;
        creators["accept quest"] = &WorldPacketTriggerContext::accept_quest;
        creators["confirm quest"] = &WorldPacketTriggerContext::confirm_quest;
        creators["quest share"] = &WorldPacketTriggerContext::quest_share;
        creators["quest update add kill"] = &WorldPacketTriggerContext::quest_update_add_kill;
        creators["quest update failed"] = &WorldPacketTriggerContext::quest_update_failed;
        creators["quest update failed timer"] = &WorldPacketTriggerContext::quest_update_failed_timer;
        creators["quest update complete"] = &WorldPacketTriggerContext::quest_update_complete;
        creators["questgiver quest details"] = &WorldPacketTriggerContext::questgiver_quest_details;
        creators["item push result"] = &WorldPacketTriggerContext::item_push_result;
        creators["use game object"] = &WorldPacketTriggerContext::use_game_object;
        creators["gossip hello"] = &WorldPacketTriggerContext::gossip_hello;
        creators["check mount state"] = &WorldPacketTriggerContext::check_mount_state;
        creators["loot response"] = &WorldPacketTriggerContext::loot_response;
        creators["master loot roll"] = &WorldPacketTriggerContext::master_loot_roll;

        // CMaNGOS parity: guild/petition automation.
        creators["guild accept"] = &WorldPacketTriggerContext::guild_accept;
        creators["petition offer"] = &WorldPacketTriggerContext::petition_offer;
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
    static Trigger* complete_quest(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "complete quest"); }
    static Trigger* accept_quest(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "accept quest"); }
    static Trigger* confirm_quest(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "confirm quest"); }
    static Trigger* quest_share(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "quest share"); }
    static Trigger* quest_update_add_kill(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "quest update add kill"); }
    static Trigger* quest_update_failed(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "quest update failed"); }
    static Trigger* quest_update_failed_timer(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "quest update failed timer"); }
    static Trigger* quest_update_complete(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "quest update complete"); }
    static Trigger* questgiver_quest_details(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "questgiver quest details"); }
    static Trigger* item_push_result(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "item push result"); }
    static Trigger* use_game_object(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "use game object"); }
    static Trigger* gossip_hello(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "gossip hello"); }
    static Trigger* check_mount_state(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "check mount state"); }
    static Trigger* loot_response(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "loot response"); }
    static Trigger* master_loot_roll(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "master loot roll"); }

    static Trigger* guild_accept(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "guild accept"); }
    static Trigger* petition_offer(PlayerbotAI* botAI) { return new WorldPacketTrigger(botAI, "petition offer"); }
};

#endif
