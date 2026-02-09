#ifndef _PLAYERBOT_ACTIONCONTEXT_H
#define _PLAYERBOT_ACTIONCONTEXT_H

#include "AttackActions.h"
#include "ActionSayHello.h"
#include "CheckMountStateAction.h"
#include "AutoMaintenanceOnLevelupAction.h"
#include "CustomStrategyEditAction.h"
#include "BattleGroundJoinAction.h"
#include "BattleGroundTactics.h"
#include "ChatActions.h"
#include "DropQuestAction.h"
#include "LootAction.h"
#include "LootRollAction.h"
#include "LootStrategyAction.h"
#include "AddLootAction.h"
#include "ChooseRpgTargetAction.h"
#include "ChooseTargetActions.h"
#include "FollowActions.h"
#include "GenericActions.h"
#include "GenericSpellActions.h"
#include "InviteToGroupAction.h"
#include "LeaveGroupAction.h"
#include "LfgActions.h"
#include "ListQuestsActions.h"
#include "MaintenanceAction.h"
#include "NewRpgAction.h"
#include "NonCombatActions.h"
#include "PositionAction.h"
#include "QueryQuestAction.h"
#include "RtiAction.h"
#include "MovementActions.h"
#include "MoveToRpgTargetAction.h"
#include "RandomBotUpdateAction.h"
#include "ReachTargetActions.h"
#include "ShareQuestAction.h"
#include "StayActions.h"
#include "StartDuelAction.h"
#include "TellTargetAction.h"
#include "TravelAction.h"
#include "ChooseTravelTargetAction.h"
#include "MoveToTravelTargetAction.h"
#include "RpgAction.h"
#include "RpgSubActions.h"
#include "ResetInstancesAction.h"
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
        creators["avoid aoe"] = &ActionContext::avoid_aoe;
        creators["travel"] = &ActionContext::travel;
        creators["choose travel target"] = &ActionContext::choose_travel_target;
        creators["move to travel target"] = &ActionContext::move_to_travel_target;
        creators["choose rpg target"] = &ActionContext::choose_rpg_target;
        creators["move to rpg target"] = &ActionContext::move_to_rpg_target;
        creators["move to loot"] = &ActionContext::move_to_loot;
        creators["rpg"] = &ActionContext::rpg;
        creators["crpg"] = &ActionContext::crpg;
        creators["check mount state"] = &ActionContext::check_mount_state;
        creators["loot"] = &ActionContext::loot;
        creators["open loot"] = &ActionContext::open_loot;
        creators["store loot"] = &ActionContext::store_loot;
        creators["release loot"] = &ActionContext::release_loot;
        creators["add loot"] = &ActionContext::add_loot;
        creators["add all loot"] = &ActionContext::add_all_loot;
        creators["add gathering loot"] = &ActionContext::add_gathering_loot;
        creators["ll"] = &ActionContext::loot_strategy;

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
        creators["join"] = &ActionContext::join;
        creators["invite"] = &ActionContext::invite;
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
        creators["rpg stay"] = &ActionContext::rpg_stay;
        creators["rpg work"] = &ActionContext::rpg_work;
        creators["rpg emote"] = &ActionContext::rpg_emote;
        creators["rpg cancel"] = &ActionContext::rpg_cancel;
        creators["rpg taxi"] = &ActionContext::rpg_taxi;
        creators["rpg discover"] = &ActionContext::rpg_discover;
        creators["rpg start quest"] = &ActionContext::rpg_start_quest;
        creators["rpg end quest"] = &ActionContext::rpg_end_quest;
        creators["rpg buy"] = &ActionContext::rpg_buy;
        creators["rpg sell"] = &ActionContext::rpg_sell;
        creators["rpg repair"] = &ActionContext::rpg_repair;
        creators["rpg train"] = &ActionContext::rpg_train;
        creators["rpg heal"] = &ActionContext::rpg_heal;
        creators["rpg home bind"] = &ActionContext::rpg_home_bind;
        creators["rpg queue bg"] = &ActionContext::rpg_queue_bg;
        creators["rpg buy petition"] = &ActionContext::rpg_buy_petition;
        creators["rpg use"] = &ActionContext::rpg_use;
        creators["rpg spell"] = &ActionContext::rpg_spell;
        creators["rpg craft"] = &ActionContext::rpg_craft;
        creators["rpg trade useful"] = &ActionContext::rpg_trade_useful;
        creators["rpg duel"] = &ActionContext::rpg_duel;
        creators["rpg mount anim"] = &ActionContext::rpg_mount_anim;
        creators["list quests"] = &ActionContext::list_quests;
        creators["query quest"] = &ActionContext::query_quest;
        creators["drop quest"] = &ActionContext::drop_quest;
        creators["clean quest log"] = &ActionContext::clean_quest_log;
        creators["share quest"] = &ActionContext::share_quest;

        creators["invite nearby"] = &ActionContext::invite_nearby;
        creators["invite guild"] = &ActionContext::invite_guild;
        creators["leave far away"] = &ActionContext::leave_far_away;
        creators["reset instances"] = &ActionContext::reset_instances;
        creators["cs"] = &ActionContext::custom_strategy_edit;
    }

private:
    static Action* hello(PlayerbotAI* botAI) { return new SayHelloAction(botAI); }
    static Action* say(PlayerbotAI* botAI) { return new SayAction(botAI); }
    static Action* talk(PlayerbotAI* botAI) { return new TalkAction(botAI); }
    static Action* emote(PlayerbotAI* botAI) { return new EmoteAction(botAI); }
    static Action* greet(PlayerbotAI* botAI) { return new GreetAction(botAI); }
    static Action* move_random(PlayerbotAI* botAI) { return new MoveRandomAction(botAI); }
    static Action* move_out_of_collision(PlayerbotAI* botAI) { return new MoveOutOfCollisionAction(botAI); }
    static Action* avoid_aoe(PlayerbotAI* botAI) { return new AvoidAoeAction(botAI); }
    static Action* travel(PlayerbotAI* botAI) { return new TravelAction(botAI); }
    static Action* choose_travel_target(PlayerbotAI* botAI) { return new ChooseTravelTargetAction(botAI); }
    static Action* move_to_travel_target(PlayerbotAI* botAI) { return new MoveToTravelTargetAction(botAI); }
    static Action* choose_rpg_target(PlayerbotAI* botAI) { return new ChooseRpgTargetAction(botAI); }
    static Action* move_to_rpg_target(PlayerbotAI* botAI) { return new MoveToRpgTargetAction(botAI); }
    static Action* move_to_loot(PlayerbotAI* botAI) { return new MoveToLootAction(botAI); }
    static Action* rpg(PlayerbotAI* botAI) { return new RpgAction(botAI); }
    static Action* crpg(PlayerbotAI* botAI) { return new CRpgAction(botAI); }
    static Action* check_mount_state(PlayerbotAI* botAI) { return new CheckMountStateAction(botAI); }
    static Action* loot(PlayerbotAI* botAI) { return new LootAction(botAI); }
    static Action* open_loot(PlayerbotAI* botAI) { return new OpenLootAction(botAI); }
    static Action* store_loot(PlayerbotAI* botAI) { return new StoreLootAction(botAI); }
    static Action* release_loot(PlayerbotAI* botAI) { return new ReleaseLootAction(botAI); }
    static Action* add_loot(PlayerbotAI* botAI) { return new AddLootAction(botAI); }
    static Action* add_all_loot(PlayerbotAI* botAI) { return new AddAllLootAction(botAI); }
    static Action* add_gathering_loot(PlayerbotAI* botAI) { return new AddGatheringLootAction(botAI); }
    static Action* loot_strategy(PlayerbotAI* botAI) { return new LootStrategyAction(botAI); }

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
    static Action* join(PlayerbotAI* botAI) { return new JoinGroupAction(botAI); }
    static Action* invite(PlayerbotAI* botAI) { return new InviteToGroupAction(botAI); }
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
    static Action* rpg_stay(PlayerbotAI* botAI) { return new RpgStayAction(botAI); }
    static Action* rpg_work(PlayerbotAI* botAI) { return new RpgWorkAction(botAI); }
    static Action* rpg_emote(PlayerbotAI* botAI) { return new RpgEmoteAction(botAI); }
    static Action* rpg_cancel(PlayerbotAI* botAI) { return new RpgCancelAction(botAI); }
    static Action* rpg_taxi(PlayerbotAI* botAI) { return new RpgTaxiAction(botAI); }
    static Action* rpg_discover(PlayerbotAI* botAI) { return new RpgDiscoverAction(botAI); }
    static Action* rpg_start_quest(PlayerbotAI* botAI) { return new RpgStartQuestAction(botAI); }
    static Action* rpg_end_quest(PlayerbotAI* botAI) { return new RpgEndQuestAction(botAI); }
    static Action* rpg_buy(PlayerbotAI* botAI) { return new RpgBuyAction(botAI); }
    static Action* rpg_sell(PlayerbotAI* botAI) { return new RpgSellAction(botAI); }
    static Action* rpg_repair(PlayerbotAI* botAI) { return new RpgRepairAction(botAI); }
    static Action* rpg_train(PlayerbotAI* botAI) { return new RpgTrainAction(botAI); }
    static Action* rpg_heal(PlayerbotAI* botAI) { return new RpgHealAction(botAI); }
    static Action* rpg_home_bind(PlayerbotAI* botAI) { return new RpgHomeBindAction(botAI); }
    static Action* rpg_queue_bg(PlayerbotAI* botAI) { return new RpgQueueBgAction(botAI); }
    static Action* rpg_buy_petition(PlayerbotAI* botAI) { return new RpgBuyPetitionAction(botAI); }
    static Action* rpg_use(PlayerbotAI* botAI) { return new RpgUseAction(botAI); }
    static Action* rpg_spell(PlayerbotAI* botAI) { return new RpgSpellAction(botAI); }
    static Action* rpg_craft(PlayerbotAI* botAI) { return new RpgCraftAction(botAI); }
    static Action* rpg_trade_useful(PlayerbotAI* botAI) { return new RpgTradeUsefulAction(botAI); }
    static Action* rpg_duel(PlayerbotAI* botAI) { return new RpgDuelAction(botAI); }
    static Action* rpg_mount_anim(PlayerbotAI* botAI) { return new RpgMountAnimAction(botAI); }
    static Action* invite_nearby(PlayerbotAI* botAI) { return new InviteNearbyToGroupAction(botAI); }
    static Action* invite_guild(PlayerbotAI* botAI) { return new InviteGuildToGroupAction(botAI); }
    static Action* leave_far_away(PlayerbotAI* botAI) { return new LeaveFarAwayAction(botAI); }
    static Action* reset_instances(PlayerbotAI* botAI) { return new ResetInstancesAction(botAI); }
    static Action* custom_strategy_edit(PlayerbotAI* botAI) { return new CustomStrategyEditAction(botAI); }
    static Action* list_quests(PlayerbotAI* botAI) { return new ListQuestsAction(botAI); }
    static Action* query_quest(PlayerbotAI* botAI) { return new QueryQuestAction(botAI); }
    static Action* drop_quest(PlayerbotAI* botAI) { return new DropQuestAction(botAI); }
    static Action* clean_quest_log(PlayerbotAI* botAI) { return new CleanQuestLogAction(botAI); }
    static Action* share_quest(PlayerbotAI* botAI) { return new ShareQuestAction(botAI); }
};

#endif
