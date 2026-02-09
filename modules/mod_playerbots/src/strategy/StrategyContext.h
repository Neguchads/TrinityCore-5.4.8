#ifndef _PLAYERBOT_STRATEGYCONTEXT_H
#define _PLAYERBOT_STRATEGYCONTEXT_H


#include "CustomStrategy.h"

#include "NamedObjectContext.h"

#include "AttackEnemyPlayersStrategy.h"
#include "AvoidMobsStrategy.h"
#include "BattlegroundStrategy.h"
#include "CastTimeStrategy.h"
#include "ChatCommandHandlerStrategy.h"
#include "ConserveManaStrategy.h"
#include "DeadStrategy.h"
#include "DebugStrategy.h"
#include "DuelStrategy.h"
#include "DpsAssistStrategy.h"
#include "EmoteStrategy.h"
#include "FleeStrategy.h"
#include "FollowMasterStrategy.h"
#include "FocusTargetStrategy.h"
#include "GrindingStrategy.h"
#include "GuardStrategy.h"
#include "GroupStrategy.h"
#include "NonCombatStrategy.h"
#include "GuildStrategy.h"
#include "KiteStrategy.h"
#include "LootNonCombatStrategy.h"
#include "LfgStrategy.h"
#include "MaintenanceStrategy.h"
#include "MarkRtiStrategy.h"
#include "MeleeCombatStrategy.h"
#include "MoveFromGroupStrategy.h"
#include "PassiveStrategy.h"
#include "PullStrategy.h"
#include "TankAssistStrategy.h"
#include "ThreatStrategy.h"
#include "HelloStrategy.h"
#include "QuestStrategies.h"
#include "RacialsStrategy.h"
#include "ReturnStrategy.h"
#include "RunawayStrategy.h"
#include "RpgStrategy.h"
#include "RTSCStrategy.h"
#include "RangedCombatStrategy.h"
#include "SayStrategy.h"
#include "StayStrategy.h"
#include "TellTargetStrategy.h"
#include "TravelStrategy.h"
#include "UseFoodStrategy.h"
#include "UsePotionsStrategy.h"
#include "WanderStrategy.h"
#include "DungeonStrategy.h"
#include "OnyxiasLairDungeonStrategies.h"
#include "MoltenCoreDungeonStrategies.h"
#include "KarazhanDungeonStrategies.h"
#include "NaxxramasDungeonStrategies.h"
#include "AzjolNerubStrategy.h"
#include "CullingOfStratholmeStrategy.h"
#include "DrakTharonKeepStrategy.h"
#include "ForgeOfSoulsStrategy.h"
#include "GundrakStrategy.h"
#include "HallsOfLightningStrategy.h"
#include "HallsOfStoneStrategy.h"
#include "NexusStrategy.h"
#include "OculusStrategy.h"
#include "OldKingdomStrategy.h"
#include "PitOfSaronStrategy.h"
#include "TrialOfTheChampionStrategy.h"
#include "UtgardeKeepStrategy.h"
#include "UtgardePinnacleStrategy.h"
#include "VioletHoldStrategy.h"
#include "RaidAq20Strategy.h"
#include "RaidBwlStrategy.h"
#include "RaidEoEStrategy.h"
#include "RaidGruulsLairStrategy.h"
#include "RaidIccStrategy.h"
#include "RaidKarazhanStrategy.h"
#include "RaidMagtheridonStrategy.h"
#include "RaidMcStrategy.h"
#include "RaidOnyxiaStrategy.h"
#include "RaidOsStrategy.h"
#include "RaidUlduarStrategy.h"
#include "RaidVoAStrategy.h"
#include "WorldPacketHandlerStrategy.h"

class StrategyContext : public NamedObjectContext<Strategy>
{
public:
    StrategyContext()
    {
        creators["default"] = &StrategyContext::world_packet;
        creators["custom"] = &StrategyContext::custom;

        creators["save mana"] = &StrategyContext::auto_save_mana;
        creators["conserve mana"] = &StrategyContext::conserve_mana;
        creators["close"] = &StrategyContext::close;
        creators["ranged"] = &StrategyContext::ranged;
        creators["dead"] = &StrategyContext::dead;
        creators["flee"] = &StrategyContext::flee;
        creators["group"] = &StrategyContext::group;
        creators["chat"] = &StrategyContext::chat;
        creators["ready check"] = &StrategyContext::ready_check;
        creators["cast time"] = &StrategyContext::cast_time;
        creators["racials"] = &StrategyContext::racials;
        creators["food"] = &StrategyContext::food;
        creators["potions"] = &StrategyContext::potions;
        creators["emote"] = &StrategyContext::emote;
        creators["loot"] = &StrategyContext::loot;
        creators["gather"] = &StrategyContext::gather;
        creators["roll"] = &StrategyContext::roll;
        creators["delayed roll"] = &StrategyContext::delayed_roll;
        creators["reveal"] = &StrategyContext::reveal;
        creators["grind"] = &StrategyContext::grind;
        creators["rpg"] = &StrategyContext::rpg;
        creators["new rpg"] = &StrategyContext::new_rpg;
        creators["rpg quest"] = &StrategyContext::rpg_quest;
        creators["rpg vendor"] = &StrategyContext::rpg_vendor;
        creators["rpg explore"] = &StrategyContext::rpg_explore;
        creators["rpg maintenance"] = &StrategyContext::rpg_maintenance;
        creators["rpg guild"] = &StrategyContext::rpg_guild;
        creators["rpg bg"] = &StrategyContext::rpg_bg;
        creators["rpg player"] = &StrategyContext::rpg_player;
        creators["rpg craft"] = &StrategyContext::rpg_craft;
        creators["rpg jump"] = &StrategyContext::rpg_jump;
        creators["travel"] = &StrategyContext::travel;
        creators["travel once"] = &StrategyContext::travel_once;
        creators["explore"] = &StrategyContext::explore;
        creators["map"] = &StrategyContext::map;
        creators["map full"] = &StrategyContext::map_full;
        creators["sit"] = &StrategyContext::sit;
        creators["wander"] = &StrategyContext::wander;
        creators["return"] = &StrategyContext::return_strategy;
        creators["mark rti"] = &StrategyContext::mark_rti;
        creators["pvp"] = &StrategyContext::pvp;
        creators["debug"] = &StrategyContext::debug;
        creators["debug action"] = &StrategyContext::debug_action;
        creators["debug move"] = &StrategyContext::debug_move;
        creators["debug rpg"] = &StrategyContext::debug_rpg;
        creators["debug spell"] = &StrategyContext::debug_spell;
        creators["debug quest"] = &StrategyContext::debug_quest;
        creators["debug travel"] = &StrategyContext::debug_travel;
        creators["debug threat"] = &StrategyContext::debug_threat;
        creators["debug mount"] = &StrategyContext::debug_mount;
        creators["debug grind"] = &StrategyContext::debug_grind;
        creators["debug loot"] = &StrategyContext::debug_loot;
        creators["debug log"] = &StrategyContext::debug_log;
        creators["debug logname"] = &StrategyContext::debug_logname;
        creators["debug llm"] = &StrategyContext::debug_llm;
        creators["debug stuck"] = &StrategyContext::debug_stuck;
        creators["debug xp"] = &StrategyContext::debug_xp;
        creators["ai chat"] = &StrategyContext::ai_chat;
        creators["guild"] = &StrategyContext::guild;
        creators["focus"] = &StrategyContext::focus;
        creators["focus heal targets"] = &StrategyContext::focus_target;
        creators["focus rti targets"] = &StrategyContext::focus_rti_targets;
        creators["kite"] = &StrategyContext::kite;
        creators["passive"] = &StrategyContext::passive;
        creators["avoid mobs"] = &StrategyContext::avoid_mobs;
        creators["avoid aoe"] = &StrategyContext::avoid_aoe;
        creators["say"] = &StrategyContext::say;
        creators["tell target"] = &StrategyContext::tell_target;
        creators["RTSC"] = &StrategyContext::rtsc;
        creators["rtsc"] = &StrategyContext::rtsc;
        creators["rtsc jump"] = &StrategyContext::rtsc_jump;
        creators["maintenance"] = &StrategyContext::maintenance;
        creators["move random"] = &StrategyContext::move_random;
        creators["wait for attack"] = &StrategyContext::wait_for_attack;
        creators["pull back"] = &StrategyContext::pull_back;
        creators["heal interrupt"] = &StrategyContext::heal_interrupt;
        creators["preheal"] = &StrategyContext::preheal;
        creators["attack tagged"] = &StrategyContext::attack_tagged;
        creators["collision"] = &StrategyContext::collision;
        creators["mount"] = &StrategyContext::mount;
        creators["silent"] = &StrategyContext::silent;
        creators["nowar"] = &StrategyContext::nowar;
        creators["glyph"] = &StrategyContext::glyph;
        creators["worldbuff"] = &StrategyContext::world_buff;
        creators["wbuff"] = &StrategyContext::world_buff;
        creators["use bobber"] = &StrategyContext::use_bobber;
        creators["master fishing"] = &StrategyContext::master_fishing;
        creators["dungeon"] = &StrategyContext::dungeon;
        creators["onyxia's lair"] = &StrategyContext::onyxias_lair;
        creators["molten core"] = &StrategyContext::molten_core;
        creators["naxxramas"] = &StrategyContext::naxxramas;
        creators["magmadar"] = &StrategyContext::magmadar;
        creators["netherspite"] = &StrategyContext::netherspite;
        creators["prince malchezaar"] = &StrategyContext::prince_malchezaar;
        creators["four horseman"] = &StrategyContext::four_horseman;
        creators["azjol'nerub"] = &StrategyContext::azjol_nerub;
        creators["culling of stratholme"] = &StrategyContext::culling_of_stratholme;
        creators["drak'tharon keep"] = &StrategyContext::drak_tharon_keep;
        creators["forge of souls"] = &StrategyContext::forge_of_souls;
        creators["gundrak"] = &StrategyContext::gundrak;
        creators["halls of lightning"] = &StrategyContext::halls_of_lightning;
        creators["halls of stone"] = &StrategyContext::halls_of_stone;
        creators["nexus"] = &StrategyContext::nexus;
        creators["oculus"] = &StrategyContext::oculus;
        creators["old kingdom"] = &StrategyContext::old_kingdom;
        creators["pit of saron"] = &StrategyContext::pit_of_saron;
        creators["trial of the champion"] = &StrategyContext::trial_of_the_champion;
        creators["utgarde keep"] = &StrategyContext::utgarde_keep;
        creators["utgarde pinnacle"] = &StrategyContext::utgarde_pinnacle;
        creators["violet hold"] = &StrategyContext::violet_hold;
        creators["wotlk-uk"] = &StrategyContext::utgarde_keep;
        creators["wotlk-nex"] = &StrategyContext::nexus;
        creators["wotlk-an"] = &StrategyContext::azjol_nerub;
        creators["wotlk-ok"] = &StrategyContext::old_kingdom;
        creators["wotlk-dtk"] = &StrategyContext::drak_tharon_keep;
        creators["wotlk-vh"] = &StrategyContext::violet_hold;
        creators["wotlk-gd"] = &StrategyContext::gundrak;
        creators["wotlk-hos"] = &StrategyContext::halls_of_stone;
        creators["wotlk-hol"] = &StrategyContext::halls_of_lightning;
        creators["wotlk-occ"] = &StrategyContext::oculus;
        creators["wotlk-up"] = &StrategyContext::utgarde_pinnacle;
        creators["wotlk-cos"] = &StrategyContext::culling_of_stratholme;
        creators["wotlk-toc"] = &StrategyContext::trial_of_the_champion;
        creators["wotlk-pos"] = &StrategyContext::pit_of_saron;
        creators["wotlk-fos"] = &StrategyContext::forge_of_souls;
        creators["wotlk-hor"] = &StrategyContext::wotlk_hor;
        creators["aq20"] = &StrategyContext::raid_aq20;
        creators["bwl"] = &StrategyContext::raid_bwl;
        creators["wotlk-eoe"] = &StrategyContext::raid_eoe;
        creators["gruulslair"] = &StrategyContext::raid_gruuls_lair;
        creators["icc"] = &StrategyContext::raid_icc;
        creators["karazhan"] = &StrategyContext::raid_karazhan;
        creators["magtheridon"] = &StrategyContext::raid_magtheridon;
        creators["moltencore"] = &StrategyContext::raid_mc;
        creators["onyxia"] = &StrategyContext::raid_onyxia;
        creators["wotlk-os"] = &StrategyContext::raid_os;
        creators["uld"] = &StrategyContext::raid_ulduar;
        creators["voa"] = &StrategyContext::raid_voa;

        creators["formation"] = &StrategyContext::combat_formation;
        creators["move from group"] = &StrategyContext::move_from_group;

        creators["tank face"] = &StrategyContext::tank_face;
        creators["threat"] = &StrategyContext::threat;
        creators["duel"] = &StrategyContext::duel;
        creators["start duel"] = &StrategyContext::start_duel;
        creators["lfg"] = &StrategyContext::lfg;
        creators["bg"] = &StrategyContext::bg;
        creators["battleground"] = &StrategyContext::battleground;
        creators["warsong"] = &StrategyContext::warsong;
        creators["alterac"] = &StrategyContext::alterac;
        creators["arathi"] = &StrategyContext::arathi;
        creators["eye"] = &StrategyContext::eye;
        creators["isle"] = &StrategyContext::isle;
        creators["arena"] = &StrategyContext::arena;
        creators["behind"] = &StrategyContext::behind;
        creators["ads"] = &StrategyContext::possible_ads;
        creators["adds"] = &StrategyContext::possible_adds;
        creators["follow jump"] = &StrategyContext::follow_jump;
        creators["chase jump"] = &StrategyContext::chase_jump;
        // -- temp
        creators["say hello"] = &StrategyContext::say_hello;
    }

private:
    // -- temp
    static Strategy* say_hello(PlayerbotAI* botAI) { return new HelloStrategy(botAI); }
    // --
    
    static Strategy* world_packet(PlayerbotAI* botAI) { return new WorldPacketHandlerStrategy(botAI); }
    static Strategy* custom(PlayerbotAI* botAI) { return new CustomStrategy(botAI); }

    static Strategy* follow_master(PlayerbotAI* botAI) { return new FollowMasterStrategy(botAI); }
    static Strategy* auto_save_mana(PlayerbotAI* botAI) { return new HealerAutoSaveManaStrategy(botAI); }
    static Strategy* conserve_mana(PlayerbotAI* botAI) { return new ConserveManaStrategy(botAI); }
    static Strategy* close(PlayerbotAI* botAI) { return new MeleeCombatStrategy(botAI); }
    static Strategy* ranged(PlayerbotAI* botAI) { return new RangedCombatStrategy(botAI); }
    static Strategy* dead(PlayerbotAI* botAI) { return new DeadStrategy(botAI); }
    static Strategy* flee(PlayerbotAI* botAI) { return new FleeStrategy(botAI); }
    static Strategy* group(PlayerbotAI* botAI) { return new GroupStrategy(botAI); }
    static Strategy* chat(PlayerbotAI* botAI) { return new ChatCommandHandlerStrategy(botAI); }
    static Strategy* ready_check(PlayerbotAI* botAI) { return new ReadyCheckStrategy(botAI); }
    static Strategy* cast_time(PlayerbotAI* botAI) { return new CastTimeStrategy(botAI); }
    static Strategy* racials(PlayerbotAI* botAI) { return new RacialsStrategy(botAI); }
    static Strategy* food(PlayerbotAI* botAI) { return new UseFoodStrategy(botAI); }
    static Strategy* potions(PlayerbotAI* botAI) { return new UsePotionsStrategy(botAI); }
    static Strategy* emote(PlayerbotAI* botAI) { return new EmoteStrategy(botAI); }
    static Strategy* loot(PlayerbotAI* botAI) { return new LootNonCombatStrategy(botAI); }
    static Strategy* gather(PlayerbotAI* botAI) { return new GatherStrategy(botAI); }
    static Strategy* roll(PlayerbotAI* botAI) { return new RollStrategy(botAI); }
    static Strategy* delayed_roll(PlayerbotAI* botAI) { return new DelayedRollStrategy(botAI); }
    static Strategy* reveal(PlayerbotAI* botAI) { return new RevealStrategy(botAI); }
    static Strategy* grind(PlayerbotAI* botAI) { return new GrindingStrategy(botAI); }
    static Strategy* rpg(PlayerbotAI* botAI) { return new RpgStrategy(botAI); }
    static Strategy* new_rpg(PlayerbotAI* botAI) { return new NewRpgStrategy(botAI); }
    static Strategy* rpg_quest(PlayerbotAI* botAI) { return new RpgQuestStrategy(botAI); }
    static Strategy* rpg_vendor(PlayerbotAI* botAI) { return new RpgVendorStrategy(botAI); }
    static Strategy* rpg_explore(PlayerbotAI* botAI) { return new RpgExploreStrategy(botAI); }
    static Strategy* rpg_maintenance(PlayerbotAI* botAI) { return new RpgMaintenanceStrategy(botAI); }
    static Strategy* rpg_guild(PlayerbotAI* botAI) { return new RpgGuildStrategy(botAI); }
    static Strategy* rpg_bg(PlayerbotAI* botAI) { return new RpgBgStrategy(botAI); }
    static Strategy* rpg_player(PlayerbotAI* botAI) { return new RpgPlayerStrategy(botAI); }
    static Strategy* rpg_craft(PlayerbotAI* botAI) { return new RpgCraftStrategy(botAI); }
    static Strategy* rpg_jump(PlayerbotAI* botAI) { return new RpgJumpStrategy(botAI); }
    static Strategy* travel(PlayerbotAI* botAI) { return new TravelStrategy(botAI); }
    static Strategy* travel_once(PlayerbotAI* botAI) { return new TravelOnceStrategy(botAI); }
    static Strategy* explore(PlayerbotAI* botAI) { return new ExploreStrategy(botAI); }
    static Strategy* map(PlayerbotAI* botAI) { return new MapStrategy(botAI); }
    static Strategy* map_full(PlayerbotAI* botAI) { return new MapFullStrategy(botAI); }
    static Strategy* sit(PlayerbotAI* botAI) { return new SitStrategy(botAI); }
    static Strategy* wander(PlayerbotAI* botAI) { return new WanderStrategy(botAI); }
    static Strategy* return_strategy(PlayerbotAI* botAI) { return new ReturnStrategy(botAI); }
    static Strategy* mark_rti(PlayerbotAI* botAI) { return new MarkRtiStrategy(botAI); }
    static Strategy* pvp(PlayerbotAI* botAI) { return new AttackEnemyPlayersStrategy(botAI); }
    static Strategy* debug(PlayerbotAI* botAI) { return new DebugStrategy(botAI); }
    static Strategy* debug_action(PlayerbotAI* botAI) { return new DebugActionStrategy(botAI); }
    static Strategy* debug_move(PlayerbotAI* botAI) { return new DebugMoveStrategy(botAI); }
    static Strategy* debug_rpg(PlayerbotAI* botAI) { return new DebugRpgStrategy(botAI); }
    static Strategy* debug_spell(PlayerbotAI* botAI) { return new DebugSpellStrategy(botAI); }
    static Strategy* debug_quest(PlayerbotAI* botAI) { return new DebugQuestStrategy(botAI); }
    static Strategy* debug_travel(PlayerbotAI* botAI) { return new DebugTravelStrategy(botAI); }
    static Strategy* debug_threat(PlayerbotAI* botAI) { return new DebugThreatStrategy(botAI); }
    static Strategy* debug_mount(PlayerbotAI* botAI) { return new DebugMountStrategy(botAI); }
    static Strategy* debug_grind(PlayerbotAI* botAI) { return new DebugGrindStrategy(botAI); }
    static Strategy* debug_loot(PlayerbotAI* botAI) { return new DebugLootStrategy(botAI); }
    static Strategy* debug_log(PlayerbotAI* botAI) { return new DebugLogStrategy(botAI); }
    static Strategy* debug_logname(PlayerbotAI* botAI) { return new DebugLogNameStrategy(botAI); }
    static Strategy* debug_llm(PlayerbotAI* botAI) { return new DebugLLMStrategy(botAI); }
    static Strategy* debug_stuck(PlayerbotAI* botAI) { return new DebugStuckStrategy(botAI); }
    static Strategy* debug_xp(PlayerbotAI* botAI) { return new DebugXpStrategy(botAI); }
    static Strategy* ai_chat(PlayerbotAI* botAI) { return new AIChatStrategy(botAI); }
    static Strategy* guild(PlayerbotAI* botAI) { return new GuildStrategy(botAI); }
    static Strategy* focus(PlayerbotAI* botAI) { return new FocusStrategy(botAI); }
    static Strategy* focus_target(PlayerbotAI* botAI) { return new FocusTargetStrategy(botAI); }
    static Strategy* focus_rti_targets(PlayerbotAI* botAI) { return new FocusRtiTargetsStrategy(botAI); }
    static Strategy* kite(PlayerbotAI* botAI) { return new KiteStrategy(botAI); }
    static Strategy* passive(PlayerbotAI* botAI) { return new PassiveStrategy(botAI); }
    static Strategy* avoid_mobs(PlayerbotAI* botAI) { return new AvoidMobsStrategy(botAI); }
    static Strategy* avoid_aoe(PlayerbotAI* botAI) { return new AvoidAoeStrategy(botAI); }
    static Strategy* say(PlayerbotAI* botAI) { return new SayStrategy(botAI); }
    static Strategy* tell_target(PlayerbotAI* botAI) { return new TellTargetStrategy(botAI); }
    static Strategy* rtsc(PlayerbotAI* botAI) { return new RTSCStrategy(botAI); }
    static Strategy* rtsc_jump(PlayerbotAI* botAI) { return new RTSCSJumptrategy(botAI); }
    static Strategy* maintenance(PlayerbotAI* botAI) { return new MaintenanceStrategy(botAI); }
    static Strategy* move_random(PlayerbotAI* botAI) { return new MoveRandomStrategy(botAI); }
    static Strategy* wait_for_attack(PlayerbotAI* botAI) { return new WaitForAttackStrategy(botAI); }
    static Strategy* pull_back(PlayerbotAI* botAI) { return new PullBackStrategy(botAI); }
    static Strategy* heal_interrupt(PlayerbotAI* botAI) { return new HealInterruptStrategy(botAI); }
    static Strategy* preheal(PlayerbotAI* botAI) { return new PreHealStrategy(botAI); }
    static Strategy* attack_tagged(PlayerbotAI* botAI) { return new AttackTaggedStrategy(botAI); }
    static Strategy* collision(PlayerbotAI* botAI) { return new CollisionStrategy(botAI); }
    static Strategy* mount(PlayerbotAI* botAI) { return new MountStrategy(botAI); }
    static Strategy* silent(PlayerbotAI* botAI) { return new SilentStrategy(botAI); }
    static Strategy* nowar(PlayerbotAI* botAI) { return new NoWarStrategy(botAI); }
    static Strategy* glyph(PlayerbotAI* botAI) { return new GlyphStrategy(botAI); }
    static Strategy* world_buff(PlayerbotAI* botAI) { return new WorldBuffStrategy(botAI); }
    static Strategy* use_bobber(PlayerbotAI* botAI) { return new UseBobberStrategy(botAI); }
    static Strategy* master_fishing(PlayerbotAI* botAI) { return new MasterFishingStrategy(botAI); }
    static Strategy* dungeon(PlayerbotAI* botAI) { return new DungeonStrategy(botAI); }
    static Strategy* onyxias_lair(PlayerbotAI* botAI) { return new OnyxiasLairDungeonStrategy(botAI); }
    static Strategy* molten_core(PlayerbotAI* botAI) { return new MoltenCoreDungeonStrategy(botAI); }
    static Strategy* naxxramas(PlayerbotAI* botAI) { return new NaxxramasDungeonStrategy(botAI); }
    static Strategy* magmadar(PlayerbotAI* botAI) { return new MagmadarFightStrategy(botAI); }
    static Strategy* netherspite(PlayerbotAI* botAI) { return new NetherspiteFightStrategy(botAI); }
    static Strategy* prince_malchezaar(PlayerbotAI* botAI) { return new PrinceMalchezaarFightStrategy(botAI); }
    static Strategy* four_horseman(PlayerbotAI* botAI) { return new FourHorsemanFightStrategy(botAI); }
    static Strategy* azjol_nerub(PlayerbotAI* botAI) { return new WotlkDungeonANStrategy(botAI); }
    static Strategy* culling_of_stratholme(PlayerbotAI* botAI) { return new WotlkDungeonCoSStrategy(botAI); }
    static Strategy* drak_tharon_keep(PlayerbotAI* botAI) { return new WotlkDungeonDTKStrategy(botAI); }
    static Strategy* forge_of_souls(PlayerbotAI* botAI) { return new WotlkDungeonFoSStrategy(botAI); }
    static Strategy* gundrak(PlayerbotAI* botAI) { return new WotlkDungeonGDStrategy(botAI); }
    static Strategy* halls_of_lightning(PlayerbotAI* botAI) { return new WotlkDungeonHoLStrategy(botAI); }
    static Strategy* halls_of_stone(PlayerbotAI* botAI) { return new WotlkDungeonHoSStrategy(botAI); }
    static Strategy* nexus(PlayerbotAI* botAI) { return new WotlkDungeonNexStrategy(botAI); }
    static Strategy* oculus(PlayerbotAI* botAI) { return new WotlkDungeonOccStrategy(botAI); }
    static Strategy* old_kingdom(PlayerbotAI* botAI) { return new WotlkDungeonOKStrategy(botAI); }
    static Strategy* pit_of_saron(PlayerbotAI* botAI) { return new WotlkDungeonPoSStrategy(botAI); }
    static Strategy* trial_of_the_champion(PlayerbotAI* botAI) { return new WotlkDungeonToCStrategy(botAI); }
    static Strategy* utgarde_keep(PlayerbotAI* botAI) { return new WotlkDungeonUKStrategy(botAI); }
    static Strategy* utgarde_pinnacle(PlayerbotAI* botAI) { return new WotlkDungeonUPStrategy(botAI); }
    static Strategy* violet_hold(PlayerbotAI* botAI) { return new WotlkDungeonVHStrategy(botAI); }
    static Strategy* wotlk_hor(PlayerbotAI* botAI) { return new DungeonStrategy(botAI); }
    static Strategy* raid_aq20(PlayerbotAI* botAI) { return new RaidAq20Strategy(botAI); }
    static Strategy* raid_bwl(PlayerbotAI* botAI) { return new RaidBwlStrategy(botAI); }
    static Strategy* raid_eoe(PlayerbotAI* botAI) { return new RaidEoEStrategy(botAI); }
    static Strategy* raid_gruuls_lair(PlayerbotAI* botAI) { return new RaidGruulsLairStrategy(botAI); }
    static Strategy* raid_icc(PlayerbotAI* botAI) { return new RaidIccStrategy(botAI); }
    static Strategy* raid_karazhan(PlayerbotAI* botAI) { return new RaidKarazhanStrategy(botAI); }
    static Strategy* raid_magtheridon(PlayerbotAI* botAI) { return new RaidMagtheridonStrategy(botAI); }
    static Strategy* raid_mc(PlayerbotAI* botAI) { return new RaidMcStrategy(botAI); }
    static Strategy* raid_onyxia(PlayerbotAI* botAI) { return new RaidOnyxiaStrategy(botAI); }
    static Strategy* raid_os(PlayerbotAI* botAI) { return new RaidOsStrategy(botAI); }
    static Strategy* raid_ulduar(PlayerbotAI* botAI) { return new RaidUlduarStrategy(botAI); }
    static Strategy* raid_voa(PlayerbotAI* botAI) { return new RaidVoAStrategy(botAI); }
    static Strategy* combat_formation(PlayerbotAI* ai) { return new CombatFormationStrategy(ai); }
    static Strategy* move_from_group(PlayerbotAI* botAI) { return new MoveFromGroupStrategy(botAI); }

    static Strategy* tank_face(PlayerbotAI* botAI) { return new TankFaceStrategy(botAI); }
    static Strategy* threat(PlayerbotAI* botAI) { return new ThreatStrategy(botAI); }
    static Strategy* duel(PlayerbotAI* botAI) { return new DuelStrategy(botAI); }
    static Strategy* start_duel(PlayerbotAI* botAI) { return new StartDuelStrategy(botAI); }
    static Strategy* lfg(PlayerbotAI* botAI) { return new LfgStrategy(botAI); }
    static Strategy* bg(PlayerbotAI* botAI) { return new BGStrategy(botAI); }
    static Strategy* battleground(PlayerbotAI* botAI) { return new BattlegroundStrategy(botAI); }
    static Strategy* warsong(PlayerbotAI* botAI) { return new WarsongStrategy(botAI); }
    static Strategy* alterac(PlayerbotAI* botAI) { return new AlteracStrategy(botAI); }
    static Strategy* arathi(PlayerbotAI* botAI) { return new ArathiStrategy(botAI); }
    static Strategy* eye(PlayerbotAI* botAI) { return new EyeStrategy(botAI); }
    static Strategy* isle(PlayerbotAI* botAI) { return new IsleStrategy(botAI); }
    static Strategy* arena(PlayerbotAI* botAI) { return new ArenaStrategy(botAI); }
    static Strategy* behind(PlayerbotAI* botAI) { return new SetBehindCombatStrategy(botAI); }
    static Strategy* possible_ads(PlayerbotAI* botAI) { return new PossibleAdsStrategy(botAI); }
    static Strategy* possible_adds(PlayerbotAI* botAI) { return new PossibleAddsStrategy(botAI); }
    static Strategy* follow_jump(PlayerbotAI* botAI) { return new FollowJumpStrategy(botAI); }
    static Strategy* chase_jump(PlayerbotAI* botAI) { return new ChaseJumpStrategy(botAI); }
};

class MovementStrategyContext : public NamedObjectContext<Strategy>
{
public:
    MovementStrategyContext() : NamedObjectContext<Strategy>(false, true)
    {
        creators["follow"] = &MovementStrategyContext::follow_master;
        creators["stay"] = &MovementStrategyContext::stay;
        creators["runaway"] = &MovementStrategyContext::runaway;
        creators["flee from adds"] = &MovementStrategyContext::flee_from_adds;
        creators["guard"] = &MovementStrategyContext::guard;
        creators["free"] = &MovementStrategyContext::free;
        creators["wander"] = &MovementStrategyContext::wander;
    }

private:
    static Strategy* guard(PlayerbotAI* botAI) { return new GuardStrategy(botAI); }
    static Strategy* follow_master(PlayerbotAI* botAI) { return new FollowMasterStrategy(botAI); }
    static Strategy* stay(PlayerbotAI* botAI) { return new StayStrategy(botAI); }
    static Strategy* runaway(PlayerbotAI* botAI) { return new RunawayStrategy(botAI); }
    static Strategy* flee_from_adds(PlayerbotAI* botAI) { return new FleeFromAddsStrategy(botAI); }
    static Strategy* free(PlayerbotAI* botAI) { return new FreeStrategy(botAI); }
    static Strategy* wander(PlayerbotAI* botAI) { return new WanderStrategy(botAI); }
};

class AssistStrategyContext : public NamedObjectContext<Strategy>
{
public:
    AssistStrategyContext() : NamedObjectContext<Strategy>(false, true)
    {
        creators["dps assist"] = &AssistStrategyContext::dps_assist;
        creators["dps aoe"] = &AssistStrategyContext::dps_aoe;
        creators["tank assist"] = &AssistStrategyContext::tank_assist;
    }

private:
    static Strategy* dps_assist(PlayerbotAI* botAI) { return new DpsAssistStrategy(botAI); }
    static Strategy* dps_aoe(PlayerbotAI* botAI) { return new DpsAoeStrategy(botAI); }
    static Strategy* tank_assist(PlayerbotAI* botAI) { return new TankAssistStrategy(botAI); }
};

class QuestStrategyContext : public NamedObjectContext<Strategy>
{
public:
    QuestStrategyContext() : NamedObjectContext<Strategy>(false, true)
    {
        creators["quest"] = &QuestStrategyContext::quest;
        creators["accept all quests"] = &QuestStrategyContext::accept_all_quests;
    }

private:
    static Strategy* quest(PlayerbotAI* botAI) { return new DefaultQuestStrategy(botAI); }
    static Strategy* accept_all_quests(PlayerbotAI* botAI) { return new AcceptAllQuestsStrategy(botAI); }
};

class FishStrategyContext : public NamedObjectContext<Strategy>
{
public:
    FishStrategyContext() : NamedObjectContext<Strategy>(false, true)
    {
        creators["fish"] = &FishStrategyContext::fish;
        creators["tfish"] = &FishStrategyContext::tfish;
    }

private:
    static Strategy* fish(PlayerbotAI* botAI) { return new FishStrategy(botAI); }
    static Strategy* tfish(PlayerbotAI* botAI) { return new TFishStrategy(botAI); }
};

#endif
