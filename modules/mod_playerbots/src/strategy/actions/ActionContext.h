#ifndef _PLAYERBOT_ACTIONCONTEXT_H
#define _PLAYERBOT_ACTIONCONTEXT_H

#include "AttackActions.h"
#include "ActionSayHello.h"
#include "CheckMountStateAction.h"
#include "AutoMaintenanceOnLevelupAction.h"
#include "BattleGroundJoinAction.h"
#include "BattleGroundTactics.h"
#include "ChatActions.h"
#include "ChooseTargetActions.h"
#include "FollowActions.h"
#include "GenericActions.h"
#include "GenericSpellActions.h"
#include "InviteToGroupAction.h"
#include "LfgActions.h"
#include "MaintenanceAction.h"
#include "NewRpgAction.h"
#include "NonCombatActions.h"
#include "PositionAction.h"
#include "RtiAction.h"
#include "MovementActions.h"
#include "RandomBotUpdateAction.h"
#include "ReachTargetActions.h"
#include "StayActions.h"
#include "StartDuelAction.h"
#include "TellTargetAction.h"
#include "TravelAction.h"
#include "ChooseTravelTargetAction.h"
#include "MoveToTravelTargetAction.h"
#include "UseItemAction.h"
#include "VehicleActions.h"

class PlayerbotAI;
class ActionContext : public NamedObjectContext<Action>
{
public:
    ActionContext()
    {
        creators["say hello"] = &ActionContext::hello;
        creators["say"] = &ActionContext::say;
        creators["talk"] = &ActionContext::talk;
        creators["emote"] = &ActionContext::emote;
        creators["greet"] = &ActionContext::greet;
        creators["move random"] = &ActionContext::move_random;
        creators["move out of collision"] = &ActionContext::move_out_of_collision;
        creators["travel"] = &ActionContext::travel;
        creators["choose travel target"] = &ActionContext::choose_travel_target;
        creators["move to travel target"] = &ActionContext::move_to_travel_target;
        creators["check mount state"] = &ActionContext::check_mount_state;

        creators["follow"] = &ActionContext::follow;
        creators["flee"] = &ActionContext::flee;
        creators["move from group"] = &ActionContext::move_from_group;
        creators["flee to master"] = &ActionContext::flee_to_master;
        creators["guard"] = &ActionContext::guard;
        creators["runaway"] = &ActionContext::runaway;
        creators["stay"] = &ActionContext::stay;
        creators["sit"] = &ActionContext::sit;

        creators["random bot update"] = &ActionContext::random_bot_update;
        creators["mark rti"] = &ActionContext::mark_rti;
        creators["attack rti target"] = &ActionContext::attack_rti_target;
        creators["auto maintenance on levelup"] = &ActionContext::auto_maintenance_on_levelup;

        creators["tank face"] = &ActionContext::tank_face;
        creators["tank assist"] = &ActionContext::tank_assist;
        creators["set facing"] = &ActionContext::set_facing;
        creators["drop target"] = &ActionContext::drop_target;
        creators["dps assist"] = &ActionContext::dps_assist;
        creators["dps aoe"] = &ActionContext::dps_aoe;
        creators["attack"] = &ActionContext::melee;
        creators["melee"] = &ActionContext::melee;
        creators["attack anything"] = &ActionContext::attack_anything;
        creators["attack duel opponent"] = &ActionContext::attack_duel_opponent;
        creators["start duel"] = &ActionContext::start_duel;
        creators["tell target"] = &ActionContext::tell_target;
        creators["tell attackers"] = &ActionContext::tell_attackers;

        creators["toggle pet spell"] = &ActionContext::toggle_pet_spell;
        creators["pet attack"] = &ActionContext::pet_attack;

        creators["reach spell"] = &ActionContext::ReachSpell;
        creators["reach melee"] = &ActionContext::ReachMelee;
        creators["reach party member to heal"] = &ActionContext::reach_party_member_to_heal;
        creators["reach party member to resurrect"] = &ActionContext::reach_party_member_to_resurrect;

        creators["lfg join"] = &ActionContext::lfg_join;
        creators["lfg leave"] = &ActionContext::lfg_leave;
        creators["lfg"] = &ActionContext::lfg;
        creators["maintenance"] = &ActionContext::maintenance;
        creators["bg join"] = &ActionContext::bg_join;
        creators["bg leave"] = &ActionContext::bg_leave;
        creators["bg status check"] = &ActionContext::bg_status_check;
        creators["free bg join"] = &ActionContext::free_bg_join;

        creators["food"] = &ActionContext::food;
        creators["drink"] = &ActionContext::drink;
        creators["healing potion"] = &ActionContext::healing_potion;
        creators["mana potion"] = &ActionContext::mana_potion;
        creators["healthstone"] = &ActionContext::healthstone;

        // BG tactics
        creators["bg tactics"] = &ActionContext::bg_tactics;
        creators["bg move to start"] = &ActionContext::bg_move_to_start;
        creators["bg reset objective force"] = &ActionContext::bg_reset_objective_force;
        creators["bg move to objective"] = &ActionContext::bg_move_to_objective;
        creators["bg check objective"] = &ActionContext::bg_check_objective;
        creators["bg protect fc"] = &ActionContext::bg_protect_fc;
        creators["bg use buff"] = &ActionContext::bg_use_buff;
        creators["bg check flag"] = &ActionContext::bg_check_flag;
        creators["arena tactics"] = &ActionContext::arena_tactics;
        creators["attack enemy flag carrier"] = &ActionContext::attack_enemy_flag_carrier;

        // Vehicles
        creators["enter vehicle"] = &ActionContext::enter_vehicle;
        creators["leave vehicle"] = &ActionContext::leave_vehicle;
        creators["hurl boulder"] = &ActionContext::hurl_boulder;
        creators["ram"] = &ActionContext::ram;
        creators["steam rush"] = &ActionContext::steam_rush;
        creators["steam blast"] = &ActionContext::steam_blast;
        creators["napalm"] = &ActionContext::napalm;
        creators["fire cannon"] = &ActionContext::fire_cannon;
        creators["incendiary rocket"] = &ActionContext::incendiary_rocket;
        creators["rocket blast"] = &ActionContext::rocket_blast;
        creators["glaive throw"] = &ActionContext::glaive_throw;

        creators["new rpg status update"] = &ActionContext::new_rpg_status_update;
        creators["new rpg go grind"] = &ActionContext::new_rpg_go_grind;
        creators["new rpg go innkeeper"] = &ActionContext::new_rpg_go_innkeeper;
        creators["new rpg move random"] = &ActionContext::new_rpg_move_random;
        creators["new rpg move npc"] = &ActionContext::new_rpg_move_npc;
        creators["rpg status"] = &ActionContext::rpg_status;

        //creators["invite nearby"] = &ActionContext::invite_nearby;
        //creators["invite guild"] = &ActionContext::invite_guild;
    }

private:
    static Action* hello(PlayerbotAI* botAI) { return new SayHelloAction(botAI); }
    static Action* say(PlayerbotAI* botAI) { return new SayAction(botAI); }
    static Action* talk(PlayerbotAI* botAI) { return new TalkAction(botAI); }
    static Action* emote(PlayerbotAI* botAI) { return new EmoteAction(botAI); }
    static Action* greet(PlayerbotAI* botAI) { return new GreetAction(botAI); }
    static Action* move_random(PlayerbotAI* botAI) { return new MoveRandomAction(botAI); }
    static Action* move_out_of_collision(PlayerbotAI* botAI) { return new MoveOutOfCollisionAction(botAI); }
    static Action* travel(PlayerbotAI* botAI) { return new TravelAction(botAI); }
    static Action* choose_travel_target(PlayerbotAI* botAI) { return new ChooseTravelTargetAction(botAI); }
    static Action* move_to_travel_target(PlayerbotAI* botAI) { return new MoveToTravelTargetAction(botAI); }
    static Action* check_mount_state(PlayerbotAI* botAI) { return new CheckMountStateAction(botAI); }

    static Action* follow(PlayerbotAI* botAI) { return new FollowAction(botAI); }
    static Action* flee(PlayerbotAI* botAI) { return new FleeAction(botAI); }
    static Action* move_from_group(PlayerbotAI* botAI) { return new MoveFromGroupAction(botAI); }
    static Action* flee_to_master(PlayerbotAI* botAI) { return new FleeToMasterAction(botAI); }
    static Action* guard(PlayerbotAI* botAI) { return new GuardAction(botAI); }
    static Action* stay(PlayerbotAI* botAI) { return new StayAction(botAI); }
    static Action* sit(PlayerbotAI* botAI) { return new SitAction(botAI); }
    static Action* runaway(PlayerbotAI* botAI) { return new RunAwayAction(botAI); }

    static Action* random_bot_update(PlayerbotAI* botAI) { return new RandomBotUpdateAction(botAI); }
    static Action* mark_rti(PlayerbotAI* botAI) { return new MarkRtiAction(botAI); }
    static Action* attack_rti_target(PlayerbotAI* botAI) { return new AttackRtiTargetAction(botAI); }
    static Action* auto_maintenance_on_levelup(PlayerbotAI* botAI) { return new AutoMaintenanceOnLevelupAction(botAI); }

    static Action* tank_face(PlayerbotAI* botAI) { return new TankFaceAction(botAI); }
    static Action* tank_assist(PlayerbotAI* botAI) { return new TankAssistAction(botAI); }
    static Action* set_facing(PlayerbotAI* botAI) { return new SetFacingTargetAction(botAI); }
    static Action* drop_target(PlayerbotAI* botAI) { return new DropTargetAction(botAI); }
    static Action* dps_assist(PlayerbotAI* botAI) { return new DpsAssistAction(botAI); }
    static Action* dps_aoe(PlayerbotAI* botAI) { return new DpsAoeAction(botAI); }
    static Action* melee(PlayerbotAI* botAI) { return new MeleeAction(botAI); }
    static Action* attack_anything(PlayerbotAI* botAI) { return new AttackAnythingAction(botAI); }
    static Action* attack_duel_opponent(PlayerbotAI* botAI) { return new AttackDuelOpponentAction(botAI); }
    static Action* start_duel(PlayerbotAI* botAI) { return new StartDuelAction(botAI); }
    static Action* tell_target(PlayerbotAI* botAI) { return new TellTargetAction(botAI); }
    static Action* tell_attackers(PlayerbotAI* botAI) { return new TellAttackersAction(botAI); }

    static Action* toggle_pet_spell(PlayerbotAI* ai) { return new TogglePetSpellAutoCastAction(ai); }
    static Action* pet_attack(PlayerbotAI* ai) { return new PetAttackAction(ai); }

    static Action* ReachSpell(PlayerbotAI* botAI) { return new ReachSpellAction(botAI); }
    static Action* ReachMelee(PlayerbotAI* botAI) { return new ReachMeleeAction(botAI); }
    static Action* reach_party_member_to_heal(PlayerbotAI* botAI) { return new ReachPartyMemberToHealAction(botAI); }
    static Action* reach_party_member_to_resurrect(PlayerbotAI* botAI) { return new ReachPartyMemberToResurrectAction(botAI); }
    static Action* lfg_join(PlayerbotAI* botAI) { return new LfgJoinAction(botAI); }
    static Action* lfg_leave(PlayerbotAI* botAI) { return new LfgLeaveAction(botAI); }
    static Action* lfg(PlayerbotAI* botAI) { return new LfgAction(botAI); }
    static Action* maintenance(PlayerbotAI* botAI) { return new MaintenanceAction(botAI); }
    static Action* bg_join(PlayerbotAI* botAI) { return new BGJoinAction(botAI); }
    static Action* bg_leave(PlayerbotAI* botAI) { return new BGLeaveAction(botAI); }
    static Action* bg_status_check(PlayerbotAI* botAI) { return new BGStatusCheckAction(botAI); }
    static Action* free_bg_join(PlayerbotAI* botAI) { return new FreeBGJoinAction(botAI); }

    static Action* drink(PlayerbotAI* botAI) { return new DrinkAction(botAI); }
    static Action* food(PlayerbotAI* botAI) { return new EatAction(botAI); }
    static Action* mana_potion(PlayerbotAI* botAI) { return new UseManaPotion(botAI); }
    static Action* healing_potion(PlayerbotAI* botAI) { return new UseHealingPotion(botAI); }
    static Action* healthstone(PlayerbotAI* botAI) { return new UseItemAction(botAI, "healthstone"); }

    // BG tactics
    static Action* bg_tactics(PlayerbotAI* botAI) { return new BGTactics(botAI); }
    static Action* bg_move_to_start(PlayerbotAI* botAI) { return new BGTactics(botAI, "move to start"); }
    static Action* bg_reset_objective_force(PlayerbotAI* botAI) { return new BGTactics(botAI, "reset objective force"); }
    static Action* bg_move_to_objective(PlayerbotAI* botAI) { return new BGTactics(botAI, "move to objective"); }
    static Action* bg_check_objective(PlayerbotAI* botAI) { return new BGTactics(botAI, "check objective"); }
    static Action* bg_protect_fc(PlayerbotAI* botAI) { return new BGTactics(botAI, "protect fc"); }
    static Action* bg_use_buff(PlayerbotAI* botAI) { return new BGTactics(botAI, "use buff"); }
    static Action* bg_check_flag(PlayerbotAI* botAI) { return new BGTactics(botAI, "check flag"); }
    static Action* arena_tactics(PlayerbotAI* botAI) { return new ArenaTactics(botAI); }
    static Action* attack_enemy_flag_carrier(PlayerbotAI* botAI) { return new AttackEnemyFlagCarrierAction(botAI); }

    // Vehicles
    static Action* enter_vehicle(PlayerbotAI* botAI) { return new EnterVehicleAction(botAI); }
    static Action* leave_vehicle(PlayerbotAI* botAI) { return new LeaveVehicleAction(botAI); }
    static Action* hurl_boulder(PlayerbotAI* botAI) { return new CastHurlBoulderAction(botAI); }
    static Action* ram(PlayerbotAI* botAI) { return new CastRamAction(botAI); }
    static Action* steam_blast(PlayerbotAI* botAI) { return new CastSteamBlastAction(botAI); }
    static Action* steam_rush(PlayerbotAI* botAI) { return new CastSteamRushAction(botAI); }
    static Action* napalm(PlayerbotAI* botAI) { return new CastNapalmAction(botAI); }
    static Action* fire_cannon(PlayerbotAI* botAI) { return new CastFireCannonAction(botAI); }
    static Action* incendiary_rocket(PlayerbotAI* botAI) { return new CastIncendiaryRocketAction(botAI); }
    static Action* rocket_blast(PlayerbotAI* botAI) { return new CastRocketBlastAction(botAI); }
    static Action* glaive_throw(PlayerbotAI* botAI) { return new CastGlaiveThrowAction(botAI); }

    static Action* new_rpg_status_update(PlayerbotAI* botAI) { return new NewRpgStatusUpdateAction(botAI); }
    static Action* new_rpg_go_grind(PlayerbotAI* botAI) { return new NewRpgGoGrindAction(botAI); }
    static Action* new_rpg_go_innkeeper(PlayerbotAI* botAI) { return new NewRpgGoInnKeeperAction(botAI); }
    static Action* new_rpg_move_random(PlayerbotAI* botAI) { return new NewRpgMoveRandomAction(botAI); }
    static Action* new_rpg_move_npc(PlayerbotAI* botAI) { return new NewRpgMoveNpcAction(botAI); }
    static Action* rpg_status(PlayerbotAI* botAI) { return new TellRpgStatusAction(botAI); }
};

#endif
