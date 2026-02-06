

#ifndef _PLAYERBOT_WORLDPACKETACTIONCONTEXT_H
#define _PLAYERBOT_WORLDPACKETACTIONCONTEXT_H


#include "NamedObjectContext.h"

#include "AcceptQuestAction.h"
#include "AcceptDuelAction.h"
#include "AcceptResurrectAction.h"
#include "AcceptInvitationAction.h"
#include "BattleGroundJoinAction.h"
#include "CheckMountStateAction.h"
#include "ReleaseSpiritAction.h"
#include "ReviveFromCorpseAction.h"
#include "LeaveGroupAction.h"
#include "LfgActions.h"
#include "LootAction.h"
#include "LootRollAction.h"
#include "QuestAction.h"

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
        creators["check mount state"] = &WorldPacketActionContext::check_mount_state;
        creators["store loot"] = &WorldPacketActionContext::store_loot;
        creators["loot roll"] = &WorldPacketActionContext::loot_roll;
        creators["master loot roll"] = &WorldPacketActionContext::master_loot_roll;
        creators["talk to quest giver"] = &WorldPacketActionContext::talk_to_quest_giver;
        creators["accept quest"] = &WorldPacketActionContext::accept_quest;
        creators["accept all quests"] = &WorldPacketActionContext::accept_all_quests;
        creators["accept quest share"] = &WorldPacketActionContext::accept_quest_share;
        creators["confirm quest"] = &WorldPacketActionContext::confirm_quest;
        creators["quest update add kill"] = &WorldPacketActionContext::quest_update_add_kill;
        creators["quest update failed"] = &WorldPacketActionContext::quest_update_failed;
        creators["quest update failed timer"] = &WorldPacketActionContext::quest_update_failed_timer;
        creators["quest update complete"] = &WorldPacketActionContext::quest_update_complete;
        creators["turn in query quest"] = &WorldPacketActionContext::turn_in_query_quest;
        creators["quest item push result"] = &WorldPacketActionContext::quest_item_push_result;
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
    static Action* check_mount_state(PlayerbotAI* botAI) { return new CheckMountStateAction(botAI); }
    static Action* store_loot(PlayerbotAI* botAI) { return new StoreLootAction(botAI); }
    static Action* loot_roll(PlayerbotAI* botAI) { return new LootRollAction(botAI); }
    static Action* master_loot_roll(PlayerbotAI* botAI) { return new MasterLootRollAction(botAI); }
    static Action* talk_to_quest_giver(PlayerbotAI* botAI) { return new TalkToQuestGiverAction(botAI); }
    static Action* accept_quest(PlayerbotAI* botAI) { return new AcceptQuestAction(botAI); }
    static Action* accept_all_quests(PlayerbotAI* botAI) { return new AcceptAllQuestsAction(botAI); }
    static Action* accept_quest_share(PlayerbotAI* botAI) { return new AcceptQuestShareAction(botAI); }
    static Action* confirm_quest(PlayerbotAI* botAI) { return new ConfirmQuestAction(botAI); }
    static Action* quest_update_add_kill(PlayerbotAI* botAI) { return new QuestUpdateAddKillAction(botAI); }
    static Action* quest_update_failed(PlayerbotAI* botAI) { return new QuestUpdateFailedAction(botAI); }
    static Action* quest_update_failed_timer(PlayerbotAI* botAI) { return new QuestUpdateFailedTimerAction(botAI); }
    static Action* quest_update_complete(PlayerbotAI* botAI) { return new QuestUpdateCompleteAction(botAI); }
    static Action* turn_in_query_quest(PlayerbotAI* botAI) { return new TurnInQueryQuestAction(botAI); }
    static Action* quest_item_push_result(PlayerbotAI* botAI) { return new QuestItemPushResultAction(botAI); }
};

#endif
