

#ifndef _PLAYERBOT_TRIGGERCONTEXT_H
#define _PLAYERBOT_TRIGGERCONTEXT_H


#include "NamedObjectContext.h"
#include "ChatCommandTrigger.h"
#include "GenericTriggers.h"
#include "HealthTriggers.h"
#include "LfgTriggers.h"
#include "LootTriggers.h"
#include "PvpTriggers.h"
#include "RangeTriggers.h"
#include "RtiTriggers.h"
#include "NewRpgTriggers.h"
#include "RpgTriggers.h"
#include "TravelTriggers.h"
#include "GuildTriggers.h"

class PlayerbotAI;
class TriggerContext : public NamedObjectContext<Trigger>
{
public:
    TriggerContext()
    {
        creators["timer"] = &TriggerContext::Timer;
        creators["timer bg"] = &TriggerContext::TimerBG;
        creators["random"] = &TriggerContext::Random;
        creators["seldom"] = &TriggerContext::seldom;
        creators["often"] = &TriggerContext::often;
        creators["always trigger"] = &TriggerContext::always;
        creators["random bot update"] = &TriggerContext::random_bot_update_trigger;
        creators["return"] = &TriggerContext::return_trigger;
        creators["no rti target"] = &TriggerContext::no_rti_target;
        creators["collision"] = &TriggerContext::collision;
        creators["new player nearby"] = &TriggerContext::new_player_nearby;

        creators["target critical health"] = &TriggerContext::TargetCriticalHealth;

        creators["critical health"] = &TriggerContext::CriticalHealth;
        creators["low health"] = &TriggerContext::LowHealth;
        creators["medium health"] = &TriggerContext::MediumHealth;
        creators["almost full health"] = &TriggerContext::AlmostFullHealth;

        creators["tank assist"] = &TriggerContext::TankAssist;
        creators["lose aggro"] = &TriggerContext::LoseAggro;
        creators["has aggro"] = &TriggerContext::HasAggro;
        creators["medium threat"] = &TriggerContext::MediumThreat;
        creators["low tank threat"] = &TriggerContext::low_tank_threat;

        creators["low mana"] = &TriggerContext::LowMana;
        creators["medium mana"] = &TriggerContext::MediumMana;
        creators["high mana"] = &TriggerContext::HighMana;
        creators["almost full mana"] = &TriggerContext::AlmostFullMana;
        creators["enough mana"] = &TriggerContext::EnoughMana;

        creators["light rage available"] = &TriggerContext::LightRageAvailable;
        creators["medium rage available"] = &TriggerContext::MediumRageAvailable;
        creators["high rage available"] = &TriggerContext::HighRageAvailable;

        creators["light energy available"] = &TriggerContext::LightEnergyAvailable;
        creators["medium energy available"] = &TriggerContext::MediumEnergyAvailable;
        creators["high energy available"] = &TriggerContext::HighEnergyAvailable;
        creators["almost full energy available"] = &TriggerContext::AlmostFullEnergyAvailable;

        creators["party member critical health"] = &TriggerContext::PartyMemberCriticalHealth;
        creators["party member low health"] = &TriggerContext::PartyMemberLowHealth;
        creators["party member medium health"] = &TriggerContext::PartyMemberMediumHealth;
        creators["party member almost full health"] = &TriggerContext::PartyMemberAlmostFullHealth;
        creators["party member dead"] = &TriggerContext::PartyMemberDead;
        creators["combat party member dead"] = &TriggerContext::CombatPartyMemberDead;
        creators["resurrect"] = &TriggerContext::PartyMemberDead;
        creators["combat party member to resurrect"] = &TriggerContext::CombatPartyMemberDead;

        creators["dead"] = &TriggerContext::Dead;
        creators["corpse near"] = &TriggerContext::corpse_near;

        creators["light aoe"] = &TriggerContext::LightAoe;
        creators["medium aoe"] = &TriggerContext::MediumAoe;
        creators["high aoe"] = &TriggerContext::HighAoe;

        creators["panic"] = &TriggerContext::panic;
        creators["outnumbered"] = &TriggerContext::outnumbered;
        creators["behind target"] = &TriggerContext::behind_target;
        creators["not behind target"] = &TriggerContext::not_behind_target;
        creators["not facing target"] = &TriggerContext::not_facing_target;

        creators["enemy out of melee"] = &TriggerContext::EnemyOutOfMelee;
        creators["enemy out of spell"] = &TriggerContext::EnemyOutOfSpell;
        creators["enemy too close for spell"] = &TriggerContext::enemy_too_close_for_spell;
        creators["enemy too close for shoot"] = &TriggerContext::enemy_too_close_for_shoot;
        creators["enemy too close for auto shot"] = &TriggerContext::enemy_too_close_for_auto_shot;
        creators["enemy too close for melee"] = &TriggerContext::enemy_too_close_for_melee;
        creators["enemy is close"] = &TriggerContext::enemy_is_close;
        creators["enemy within melee"] = &TriggerContext::enemy_within_melee;
        creators["not dps target active"] = &TriggerContext::not_dps_target_active;
        creators["not dps aoe target active"] = &TriggerContext::not_dps_aoe_target_active;

        creators["party member to heal out of spell range"] = &TriggerContext::party_member_to_heal_out_of_spell_range;
        creators["no target"] = &TriggerContext::NoTarget;
        creators["target changed"] = &TriggerContext::target_changed;
        creators["invalid target"] = &TriggerContext::invalid_target;

        creators["no attackers"] = &TriggerContext::NoAttackers;
        creators["has attackers"] = &TriggerContext::has_attackers;
        creators["no possible targets"] = &TriggerContext::no_possible_targets;
        creators["possible adds"] = &TriggerContext::possible_adds;

        creators["no pet"] = &TriggerContext::no_pet;
        creators["has pet"] = &TriggerContext::has_pet;
        creators["pet attack"] = &TriggerContext::pet_attack;

        creators["protect party member"] = &TriggerContext::protect_party_member;
        creators["lfg proposal active"] = &TriggerContext::lfg_proposal_active;
        creators["lfg teleport in"] = &TriggerContext::lfg_teleport_in;
        creators["lfg"] = &TriggerContext::lfg;
        creators["summon"] = &TriggerContext::summon;
        creators["group summon"] = &TriggerContext::group_summon;
        creators["revive"] = &TriggerContext::revive;
        creators["group revive"] = &TriggerContext::group_revive;
        creators["leave large guild"] = &TriggerContext::leave_large_guild;
        creators["petition signed"] = &TriggerContext::petition_signed;
        creators["buy tabard"] = &TriggerContext::buy_tabard;
        creators["maintenance"] = &TriggerContext::maintenance;
        creators["bank"] = &TriggerContext::bank;
        creators["gb"] = &TriggerContext::gb;
        creators["guild bank"] = &TriggerContext::guild_bank;
        creators["guild invite"] = &TriggerContext::guild_invite;
        creators["guild join"] = &TriggerContext::guild_join;
        creators["guild promote"] = &TriggerContext::guild_promote;
        creators["guild demote"] = &TriggerContext::guild_demote;
        creators["guild remove"] = &TriggerContext::guild_remove;
        creators["guild leader"] = &TriggerContext::guild_leader;
        creators["guild leave"] = &TriggerContext::guild_leave;
        creators["mail"] = &TriggerContext::mail;
        creators["sendmail"] = &TriggerContext::sendmail;
        creators["ah"] = &TriggerContext::ah;
        creators["help"] = &TriggerContext::help;
        creators["reset"] = &TriggerContext::reset;
        creators["reset ai"] = &TriggerContext::reset_ai;
        creators["reset ai soft"] = &TriggerContext::reset_ai_soft;
        creators["reset strats"] = &TriggerContext::reset_strats;
        creators["reset values"] = &TriggerContext::reset_values;
        creators["save ai"] = &TriggerContext::save_ai;
        creators["load ai"] = &TriggerContext::load_ai;
        creators["list ai"] = &TriggerContext::list_ai;
        creators["cs"] = &TriggerContext::cs;
        creators["q"] = &TriggerContext::q;
        creators["quests"] = &TriggerContext::quests;
        creators["drop"] = &TriggerContext::drop;
        creators["share"] = &TriggerContext::share;
        creators["loot available"] = &TriggerContext::loot_available;
        creators["far from loot target"] = &TriggerContext::far_from_loot_target;
        creators["can loot"] = &TriggerContext::can_loot;

        creators["bg waiting"] = &TriggerContext::bg_waiting;
        creators["bg active"] = &TriggerContext::bg_active;
        creators["bg invite active"] = &TriggerContext::bg_invite_active;
        creators["player has flag"] = &TriggerContext::player_has_flag;
        creators["player has no flag"] = &TriggerContext::player_has_no_flag;
        creators["enemy flagcarrier near"] = &TriggerContext::enemy_flagcarrier_near;
        creators["team flagcarrier near"] = &TriggerContext::team_flagcarrier_near;
        creators["team has flag"] = &TriggerContext::team_has_flag;
        creators["enemy team has flag"] = &TriggerContext::enemy_team_has_flag;
        creators["in Battleground"] = &TriggerContext::in_battleground;
        creators["inside bg"] = &TriggerContext::inside_bg;
        creators["in Battleground without flag"] = &TriggerContext::in_battleground_without_flag;
        creators["wants in bg"] = &TriggerContext::wants_in_bg;
        creators["vehicle near"] = &TriggerContext::vehicle_near;
        creators["in vehicle"] = &TriggerContext::in_vehicle;
        creators["alliance no snowfall gy"] = &TriggerContext::alliance_no_snowfall_gy;
        creators["go grind status"] = &TriggerContext::go_grind_status;
        creators["go innkeeper status"] = &TriggerContext::go_innkeeper_status;
        creators["near random status"] = &TriggerContext::near_random_status;
        creators["near npc status"] = &TriggerContext::near_npc_status;
        creators["no rpg target"] = &TriggerContext::no_rpg_target;
        creators["has rpg target"] = &TriggerContext::has_rpg_target;
        creators["far from rpg target"] = &TriggerContext::far_from_rpg_target;
        creators["near rpg target"] = &TriggerContext::near_rpg_target;
        creators["rpg"] = &TriggerContext::rpg;
        creators["rpg taxi"] = &TriggerContext::rpg_taxi;
        creators["rpg discover"] = &TriggerContext::rpg_discover;
        creators["rpg start quest"] = &TriggerContext::rpg_start_quest;
        creators["rpg end quest"] = &TriggerContext::rpg_end_quest;
        creators["rpg buy"] = &TriggerContext::rpg_buy;
        creators["rpg sell"] = &TriggerContext::rpg_sell;
        creators["rpg repair"] = &TriggerContext::rpg_repair;
        creators["rpg train"] = &TriggerContext::rpg_train;
        creators["rpg heal"] = &TriggerContext::rpg_heal;
        creators["rpg home bind"] = &TriggerContext::rpg_home_bind;
        creators["rpg queue bg"] = &TriggerContext::rpg_queue_bg;
        creators["rpg buy petition"] = &TriggerContext::rpg_buy_petition;
        creators["rpg use"] = &TriggerContext::rpg_use;
        creators["rpg spell"] = &TriggerContext::rpg_spell;
        creators["rpg craft"] = &TriggerContext::rpg_craft;
        creators["rpg trade useful"] = &TriggerContext::rpg_trade_useful;
        creators["rpg duel"] = &TriggerContext::rpg_duel;
        creators["no travel target"] = &TriggerContext::no_travel_target;
        creators["far from travel target"] = &TriggerContext::far_from_travel_target;
    }

private:
    static Trigger* Timer(PlayerbotAI* botAI) { return new TimerTrigger(botAI); }
    static Trigger* TimerBG(PlayerbotAI* botAI) { return new TimerBGTrigger(botAI); }
    static Trigger* Random(PlayerbotAI* botAI) { return new RandomTrigger(botAI, "random", 20); }
    static Trigger* seldom(PlayerbotAI* botAI) { return new RandomTrigger(botAI, "seldom", 300); }
    static Trigger* often(PlayerbotAI* botAI) { return new RandomTrigger(botAI, "often", 5); }
    static Trigger* always(PlayerbotAI* botAI) { return new AlwaysTrigger(botAI); }
    static Trigger* random_bot_update_trigger(PlayerbotAI* botAI) { return new RandomBotUpdateTrigger(botAI); }
    static Trigger* return_trigger(PlayerbotAI* botAI) { return new ReturnTrigger(botAI); }
    static Trigger* no_rti_target(PlayerbotAI* botAI) { return new NoRtiTrigger(botAI); }
    static Trigger* collision(PlayerbotAI* botAI) { return new CollisionTrigger(botAI); }
    static Trigger* new_player_nearby(PlayerbotAI* botAI) { return new NewPlayerNearbyTrigger(botAI); }

    static Trigger* LowHealth(PlayerbotAI* botAI) { return new LowHealthTrigger(botAI); }
    static Trigger* MediumHealth(PlayerbotAI* botAI) { return new MediumHealthTrigger(botAI); }
    static Trigger* AlmostFullHealth(PlayerbotAI* botAI) { return new AlmostFullHealthTrigger(botAI); }
    static Trigger* CriticalHealth(PlayerbotAI* botAI) { return new CriticalHealthTrigger(botAI); }
    static Trigger* TargetCriticalHealth(PlayerbotAI* botAI) { return new TargetCriticalHealthTrigger(botAI); }
    static Trigger* PartyMemberDead(PlayerbotAI* botAI) { return new PartyMemberDeadTrigger(botAI); }
    static Trigger* CombatPartyMemberDead(PlayerbotAI* botAI) { return new CombatPartyMemberDeadTrigger(botAI); }
    static Trigger* PartyMemberLowHealth(PlayerbotAI* botAI) { return new PartyMemberLowHealthTrigger(botAI); }
    static Trigger* PartyMemberMediumHealth(PlayerbotAI* botAI) { return new PartyMemberMediumHealthTrigger(botAI); }
    static Trigger* PartyMemberAlmostFullHealth(PlayerbotAI* botAI) { return new PartyMemberAlmostFullHealthTrigger(botAI); }
    static Trigger* LightRageAvailable(PlayerbotAI* botAI) { return new LightRageAvailableTrigger(botAI); }
    static Trigger* MediumRageAvailable(PlayerbotAI* botAI) { return new MediumRageAvailableTrigger(botAI); }
    static Trigger* HighRageAvailable(PlayerbotAI* botAI) { return new HighRageAvailableTrigger(botAI); }
    static Trigger* LightEnergyAvailable(PlayerbotAI* botAI) { return new LightEnergyAvailableTrigger(botAI); }
    static Trigger* MediumEnergyAvailable(PlayerbotAI* botAI) { return new MediumEnergyAvailableTrigger(botAI); }
    static Trigger* HighEnergyAvailable(PlayerbotAI* botAI) { return new HighEnergyAvailableTrigger(botAI); }
    static Trigger* AlmostFullEnergyAvailable(PlayerbotAI* botAI) { return new EnergyAvailable(botAI, 90); }
    static Trigger* generic_boost(PlayerbotAI* botAI) { return new GenericBoostTrigger(botAI); }
    static Trigger* PartyMemberCriticalHealth(PlayerbotAI* botAI) { return new PartyMemberCriticalHealthTrigger(botAI); }
    static Trigger* LowMana(PlayerbotAI* botAI) { return new LowManaTrigger(botAI); }
    static Trigger* MediumMana(PlayerbotAI* botAI) { return new MediumManaTrigger(botAI); }
    static Trigger* HighMana(PlayerbotAI* botAI) { return new HighManaTrigger(botAI); }
    static Trigger* AlmostFullMana(PlayerbotAI* botAI) { return new AlmostFullManaTrigger(botAI); }
    static Trigger* EnoughMana(PlayerbotAI* botAI) { return new EnoughManaTrigger(botAI); }

    static Trigger* TankAssist(PlayerbotAI* botAI) { return new TankAssistTrigger(botAI); }
    static Trigger* LoseAggro(PlayerbotAI* botAI) { return new LoseAggroTrigger(botAI); }
    static Trigger* HasAggro(PlayerbotAI* botAI) { return new HasAggroTrigger(botAI); }
    static Trigger* MediumThreat(PlayerbotAI* botAI) { return new MediumThreatTrigger(botAI); }
    static Trigger* low_tank_threat(PlayerbotAI* botAI) { return new LowTankThreatTrigger(botAI); }

    static Trigger* Dead(PlayerbotAI* botAI) { return new DeadTrigger(botAI); }
    static Trigger* corpse_near(PlayerbotAI* botAI) { return new CorpseNearTrigger(botAI); }

    static Trigger* LightAoe(PlayerbotAI* botAI) { return new LightAoeTrigger(botAI); }
    static Trigger* MediumAoe(PlayerbotAI* botAI) { return new MediumAoeTrigger(botAI); }
    static Trigger* HighAoe(PlayerbotAI* botAI) { return new HighAoeTrigger(botAI); }

    static Trigger* panic(PlayerbotAI* botAI) { return new PanicTrigger(botAI); }
    static Trigger* outnumbered(PlayerbotAI* botAI) { return new OutNumberedTrigger(botAI); }

    static Trigger* behind_target(PlayerbotAI* botAI) { return new IsBehindTargetTrigger(botAI); }
    static Trigger* not_behind_target(PlayerbotAI* botAI) { return new IsNotBehindTargetTrigger(botAI); }
    static Trigger* not_facing_target(PlayerbotAI* botAI) { return new IsNotFacingTargetTrigger(botAI); }
    static Trigger* EnemyOutOfMelee(PlayerbotAI* botAI) { return new EnemyOutOfMeleeTrigger(botAI); }
    static Trigger* EnemyOutOfSpell(PlayerbotAI* botAI) { return new EnemyOutOfSpellRangeTrigger(botAI); }
    static Trigger* enemy_too_close_for_spell(PlayerbotAI* botAI) { return new EnemyTooCloseForSpellTrigger(botAI); }
    static Trigger* enemy_too_close_for_auto_shot(PlayerbotAI* botAI) { return new EnemyTooCloseForAutoShotTrigger(botAI); }
    static Trigger* enemy_too_close_for_shoot(PlayerbotAI* botAI) { return new EnemyTooCloseForShootTrigger(botAI); }
    static Trigger* enemy_too_close_for_melee(PlayerbotAI* botAI) { return new EnemyTooCloseForMeleeTrigger(botAI); }
    static Trigger* enemy_is_close(PlayerbotAI* botAI) { return new EnemyIsCloseTrigger(botAI); }
    static Trigger* enemy_within_melee(PlayerbotAI* botAI) { return new EnemyWithinMeleeTrigger(botAI); }
    static Trigger* not_dps_target_active(PlayerbotAI* botAI) { return new NotDpsTargetActiveTrigger(botAI); }
    static Trigger* not_dps_aoe_target_active(PlayerbotAI* botAI) { return new NotDpsAoeTargetActiveTrigger(botAI); }

    static Trigger* party_member_to_heal_out_of_spell_range(PlayerbotAI* botAI) { return new PartyMemberToHealOutOfSpellRangeTrigger(botAI); }
    static Trigger* target_changed(PlayerbotAI* botAI) { return new TargetChangedTrigger(botAI); }
    static Trigger* NoTarget(PlayerbotAI* botAI) { return new NoTargetTrigger(botAI); }
    static Trigger* invalid_target(PlayerbotAI* botAI) { return new InvalidTargetTrigger(botAI); }

    static Trigger* NoAttackers(PlayerbotAI* botAI) { return new NoAttackersTrigger(botAI); }
    static Trigger* has_attackers(PlayerbotAI* botAI) { return new HasAttackersTrigger(botAI); }
    static Trigger* no_possible_targets(PlayerbotAI* botAI) { return new NoPossibleTargetsTrigger(botAI); }
    static Trigger* possible_adds(PlayerbotAI* botAI) { return new PossibleAddsTrigger(botAI); }

    static Trigger* no_pet(PlayerbotAI* botAI) { return new NoPetTrigger(botAI); }
    static Trigger* has_pet(PlayerbotAI* botAI) { return new HasPetTrigger(botAI); }
    static Trigger* pet_attack(PlayerbotAI* botAI) { return new PetAttackTrigger(botAI); }

    static Trigger* protect_party_member(PlayerbotAI* botAI) { return new ProtectPartyMemberTrigger(botAI); }
    static Trigger* lfg_proposal_active(PlayerbotAI* botAI) { return new LfgProposalActiveTrigger(botAI); }
    static Trigger* lfg_teleport_in(PlayerbotAI* botAI) { return new LfgTeleportInTrigger(botAI); }
    static Trigger* lfg(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "lfg"); }
    static Trigger* summon(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "summon"); }
    static Trigger* group_summon(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "group summon"); }
    static Trigger* revive(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "revive"); }
    static Trigger* group_revive(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "group revive"); }
    static Trigger* leave_large_guild(PlayerbotAI* botAI) { return new LeaveLargeGuildTrigger(botAI); }
    static Trigger* petition_signed(PlayerbotAI* botAI) { return new PetitionTurnInTrigger(botAI); }
    static Trigger* buy_tabard(PlayerbotAI* botAI) { return new BuyTabardTrigger(botAI); }
    static Trigger* maintenance(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "maintenance"); }
    static Trigger* bank(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "bank"); }
    static Trigger* gb(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "gb"); }
    static Trigger* guild_bank(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "guild bank"); }
    static Trigger* guild_invite(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "guild invite"); }
    static Trigger* guild_join(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "guild join"); }
    static Trigger* guild_promote(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "guild promote"); }
    static Trigger* guild_demote(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "guild demote"); }
    static Trigger* guild_remove(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "guild remove"); }
    static Trigger* guild_leader(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "guild leader"); }
    static Trigger* guild_leave(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "guild leave"); }
    static Trigger* mail(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "mail"); }
    static Trigger* sendmail(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "sendmail"); }
    static Trigger* ah(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "ah"); }
    static Trigger* help(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "help"); }
    static Trigger* reset(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "reset"); }
    static Trigger* reset_ai(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "reset ai"); }
    static Trigger* reset_ai_soft(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "reset ai soft"); }
    static Trigger* reset_strats(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "reset strats"); }
    static Trigger* reset_values(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "reset values"); }
    static Trigger* save_ai(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "save ai"); }
    static Trigger* load_ai(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "load ai"); }
    static Trigger* list_ai(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "list ai"); }
    static Trigger* cs(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "cs"); }
    static Trigger* q(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "q"); }
    static Trigger* quests(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "quests"); }
    static Trigger* drop(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "drop"); }
    static Trigger* share(PlayerbotAI* botAI) { return new ChatCommandTrigger(botAI, "share"); }
    static Trigger* loot_available(PlayerbotAI* botAI) { return new LootAvailableTrigger(botAI); }
    static Trigger* far_from_loot_target(PlayerbotAI* botAI) { return new FarFromCurrentLootTrigger(botAI); }
    static Trigger* can_loot(PlayerbotAI* botAI) { return new CanLootTrigger(botAI); }

    static Trigger* bg_waiting(PlayerbotAI* botAI) { return new BgWaitingTrigger(botAI); }
    static Trigger* bg_active(PlayerbotAI* botAI) { return new BgActiveTrigger(botAI); }
    static Trigger* bg_invite_active(PlayerbotAI* botAI) { return new BgInviteActiveTrigger(botAI); }
    static Trigger* player_has_flag(PlayerbotAI* botAI) { return new PlayerHasFlag(botAI); }
    static Trigger* player_has_no_flag(PlayerbotAI* botAI) { return new PlayerHasNoFlag(botAI); }
    static Trigger* enemy_flagcarrier_near(PlayerbotAI* botAI) { return new EnemyFlagCarrierNear(botAI); }
    static Trigger* team_flagcarrier_near(PlayerbotAI* botAI) { return new TeamFlagCarrierNear(botAI); }
    static Trigger* team_has_flag(PlayerbotAI* botAI) { return new TeamHasFlag(botAI); }
    static Trigger* enemy_team_has_flag(PlayerbotAI* botAI) { return new EnemyTeamHasFlag(botAI); }
    static Trigger* in_battleground(PlayerbotAI* botAI) { return new PlayerIsInBattleground(botAI); }
    static Trigger* inside_bg(PlayerbotAI* botAI) { return new InsideBGTrigger(botAI); }
    static Trigger* in_battleground_without_flag(PlayerbotAI* botAI) { return new PlayerIsInBattlegroundWithoutFlag(botAI); }
    static Trigger* wants_in_bg(PlayerbotAI* botAI) { return new PlayerWantsInBattlegroundTrigger(botAI); }
    static Trigger* vehicle_near(PlayerbotAI* botAI) { return new VehicleNearTrigger(botAI); }
    static Trigger* in_vehicle(PlayerbotAI* botAI) { return new InVehicleTrigger(botAI); }
    static Trigger* alliance_no_snowfall_gy(PlayerbotAI* botAI) { return new AllianceNoSnowfallGY(botAI); }
    static Trigger* go_grind_status(PlayerbotAI* botAI) { return new NewRpgStatusTrigger(botAI, NewRpgStatus::GO_GRIND); }
    static Trigger* go_innkeeper_status(PlayerbotAI* botAI) { return new NewRpgStatusTrigger(botAI, NewRpgStatus::GO_INNKEEPER); }
    static Trigger* near_random_status(PlayerbotAI* botAI) { return new NewRpgStatusTrigger(botAI, NewRpgStatus::NEAR_RANDOM); }
    static Trigger* near_npc_status(PlayerbotAI* botAI) { return new NewRpgStatusTrigger(botAI, NewRpgStatus::NEAR_NPC); }
    static Trigger* no_rpg_target(PlayerbotAI* botAI) { return new NoRpgTargetTrigger(botAI); }
    static Trigger* has_rpg_target(PlayerbotAI* botAI) { return new HasRpgTargetTrigger(botAI); }
    static Trigger* far_from_rpg_target(PlayerbotAI* botAI) { return new FarFromRpgTargetTrigger(botAI); }
    static Trigger* near_rpg_target(PlayerbotAI* botAI) { return new NearRpgTargetTrigger(botAI); }
    static Trigger* rpg(PlayerbotAI* botAI) { return new RpgTrigger(botAI); }
    static Trigger* rpg_taxi(PlayerbotAI* botAI) { return new RpgTaxiTrigger(botAI); }
    static Trigger* rpg_discover(PlayerbotAI* botAI) { return new RpgDiscoverTrigger(botAI); }
    static Trigger* rpg_start_quest(PlayerbotAI* botAI) { return new RpgStartQuestTrigger(botAI); }
    static Trigger* rpg_end_quest(PlayerbotAI* botAI) { return new RpgEndQuestTrigger(botAI); }
    static Trigger* rpg_buy(PlayerbotAI* botAI) { return new RpgBuyTrigger(botAI); }
    static Trigger* rpg_sell(PlayerbotAI* botAI) { return new RpgSellTrigger(botAI); }
    static Trigger* rpg_repair(PlayerbotAI* botAI) { return new RpgRepairTrigger(botAI); }
    static Trigger* rpg_train(PlayerbotAI* botAI) { return new RpgTrainTrigger(botAI); }
    static Trigger* rpg_heal(PlayerbotAI* botAI) { return new RpgHealTrigger(botAI); }
    static Trigger* rpg_home_bind(PlayerbotAI* botAI) { return new RpgHomeBindTrigger(botAI); }
    static Trigger* rpg_queue_bg(PlayerbotAI* botAI) { return new RpgQueueBGTrigger(botAI); }
    static Trigger* rpg_buy_petition(PlayerbotAI* botAI) { return new RpgBuyPetitionTrigger(botAI); }
    static Trigger* rpg_use(PlayerbotAI* botAI) { return new RpgUseTrigger(botAI); }
    static Trigger* rpg_spell(PlayerbotAI* botAI) { return new RpgSpellTrigger(botAI); }
    static Trigger* rpg_craft(PlayerbotAI* botAI) { return new RpgCraftTrigger(botAI); }
    static Trigger* rpg_trade_useful(PlayerbotAI* botAI) { return new RpgTradeUsefulTrigger(botAI); }
    static Trigger* rpg_duel(PlayerbotAI* botAI) { return new RpgDuelTrigger(botAI); }
    static Trigger* no_travel_target(PlayerbotAI* botAI) { return new NoTravelTargetTrigger(botAI); }
    static Trigger* far_from_travel_target(PlayerbotAI* botAI) { return new FarFromTravelTargetTrigger(botAI); }
};

#endif
