

#include "HunterAiObjectContext.h"

#include "BeastMasteryHunterStrategy.h"
#include "DpsHunterStrategy.h"
#include "GenericHunterNonCombatStrategy.h"
#include "HunterStrategy.h"
#include "HunterActions.h"
#include "HunterBuffStrategies.h"
#include "HunterTriggers.h"
#include "MarksmanshipHunterStrategy.h"
#include "NamedObjectContext.h"
#include "Playerbots.h"
#include "SurvivalHunterStrategy.h"

class HunterStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    HunterStrategyFactoryInternal()
    {
        creators["dps"] = &HunterStrategyFactoryInternal::dps;
        creators["nc"] = &HunterStrategyFactoryInternal::nc;
        creators["aoe"] = &HunterStrategyFactoryInternal::aoe;
        creators["dps debuff"] = &HunterStrategyFactoryInternal::dps_debuff;
        creators["boost"] = &HunterStrategyFactoryInternal::boost;
        creators["pet"] = &HunterStrategyFactoryInternal::pet;
        creators["cc"] = &HunterStrategyFactoryInternal::cc;
        creators["beast"] = &HunterStrategyFactoryInternal::beast;
        creators["marks"] = &HunterStrategyFactoryInternal::marks;
        creators["surv"] = &HunterStrategyFactoryInternal::surv;
        creators["hunter"] = &HunterStrategyFactoryInternal::hunter;
    }

private:
    static Strategy* aoe(PlayerbotAI* botAI) { return new DpsAoeHunterStrategy(botAI); }
    static Strategy* dps(PlayerbotAI* botAI) { return new DpsHunterStrategy(botAI); }
    static Strategy* nc(PlayerbotAI* botAI) { return new GenericHunterNonCombatStrategy(botAI); }
    static Strategy* dps_debuff(PlayerbotAI* botAI) { return new DpsHunterDebuffStrategy(botAI); }
    static Strategy* boost(PlayerbotAI* botAI) { return new HunterBoostStrategy(botAI); }
    static Strategy* pet(PlayerbotAI* botAI) { return new HunterPetStrategy(botAI); }
    static Strategy* cc(PlayerbotAI* botAI) { return new HunterCcStrategy(botAI); }
    static Strategy* beast(PlayerbotAI* botAI) { return new BeastMasteryHunterStrategy(botAI); }
    static Strategy* marks(PlayerbotAI* botAI) { return new MarksmanshipHunterStrategy(botAI); }
    static Strategy* surv(PlayerbotAI* botAI) { return new SurvivalHunterStrategy(botAI); }
    static Strategy* hunter(PlayerbotAI* botAI) { return new HunterStrategy(botAI); }
};

class HunterBuffStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    HunterBuffStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["bspeed"] = &HunterBuffStrategyFactoryInternal::bspeed;
        creators["bdps"] = &HunterBuffStrategyFactoryInternal::bdps;
        creators["bmana"] = &HunterBuffStrategyFactoryInternal::bmana;
        creators["rnature"] = &HunterBuffStrategyFactoryInternal::rnature;
    }

private:
    static Strategy* bspeed(PlayerbotAI* botAI) { return new HunterBuffSpeedStrategy(botAI); }
    static Strategy* bdps(PlayerbotAI* botAI) { return new HunterBuffDpsStrategy(botAI); }
    static Strategy* bmana(PlayerbotAI* botAI) { return new HunterBuffManaStrategy(botAI); }
    static Strategy* rnature(PlayerbotAI* botAI) { return new HunterNatureResistanceStrategy(botAI); }
};

class HunterTriggerFactoryInternal : public NamedObjectContext<Trigger>
{
public:
    HunterTriggerFactoryInternal()
    {
        creators["black arrow"] = &HunterTriggerFactoryInternal::black_arrow;
        creators["serpent sting"] = &HunterTriggerFactoryInternal::serpent_sting;
        creators["hunter's mark"] = &HunterTriggerFactoryInternal::hunters_mark;
        creators["serpent sting on attacker"] = &HunterTriggerFactoryInternal::serpent_sting_on_attacker;
        creators["concussive shot on snare target"] = &HunterTriggerFactoryInternal::concussive_shot_on_snare_target;
        creators["focus fire"] = &HunterTriggerFactoryInternal::focus_fire;
        creators["lock and load"] = &HunterTriggerFactoryInternal::lock_and_load;
        creators["fire!"] = &HunterTriggerFactoryInternal::fire;
        creators["aspect of the hawk"] = &HunterTriggerFactoryInternal::aspect_of_the_hawk;
        creators["aspect of the monkey"] = &HunterTriggerFactoryInternal::aspect_of_the_monkey;
        creators["aspect of the wild"] = &HunterTriggerFactoryInternal::aspect_of_the_wild;
        creators["aspect of the viper"] = &HunterTriggerFactoryInternal::aspect_of_the_viper;
        creators["aspect of the pack"] = &HunterTriggerFactoryInternal::aspect_of_the_pack;
        creators["trueshot aura"] = &HunterTriggerFactoryInternal::trueshot_aura;
        creators["no track"] = &HunterTriggerFactoryInternal::no_track;
    }

private:
    static Trigger* black_arrow(PlayerbotAI* botAI) { return new HunterBlackArrowTrigger(botAI); }
    static Trigger* serpent_sting(PlayerbotAI* botAI) { return new HunterSerpentStingTrigger(botAI); }
    static Trigger* hunters_mark(PlayerbotAI* botAI) { return new HuntersMarkTrigger(botAI); }
    static Trigger* serpent_sting_on_attacker(PlayerbotAI* botAI) { return new DebuffOnAttackerTrigger(botAI, "serpent sting", true); }
    static Trigger* concussive_shot_on_snare_target(PlayerbotAI* botAI) { return new SnareTargetTrigger(botAI, "concussive shot"); }
    static Trigger* focus_fire(PlayerbotAI* botAI) { return new HunterFocusFireTrigger(botAI); }
    static Trigger* lock_and_load(PlayerbotAI* botAI) { return new HunterLockAndLoadTrigger(botAI); }
    static Trigger* fire(PlayerbotAI* botAI) { return new HunterFireTrigger(botAI); }
    static Trigger* aspect_of_the_hawk(PlayerbotAI* botAI) { return new HunterAspectOfTheHawkTrigger(botAI); }
    static Trigger* aspect_of_the_monkey(PlayerbotAI* botAI) { return new HunterAspectOfTheMonkeyTrigger(botAI); }
    static Trigger* aspect_of_the_wild(PlayerbotAI* botAI) { return new HunterAspectOfTheWildTrigger(botAI); }
    static Trigger* aspect_of_the_viper(PlayerbotAI* botAI) { return new HunterAspectOfTheViperTrigger(botAI); }
    static Trigger* aspect_of_the_pack(PlayerbotAI* botAI) { return new HunterAspectOfThePackTrigger(botAI); }
    static Trigger* trueshot_aura(PlayerbotAI* botAI) { return new TrueshotAuraTrigger(botAI); }
    static Trigger* no_track(PlayerbotAI* botAI) { return new NoTrackTrigger(botAI); }
};

class HunterAiObjectContextInternal : public NamedObjectContext<Action>
{
public:
    HunterAiObjectContextInternal()
    {
        creators["auto shot"] = &HunterAiObjectContextInternal::auto_shot;
        creators["aimed shot"] = &HunterAiObjectContextInternal::aimed_shot;
        creators["chimera shot"] = &HunterAiObjectContextInternal::chimera_shot;
        creators["explosive shot"] = &HunterAiObjectContextInternal::explosive_shot;
        creators["arcane shot"] = &HunterAiObjectContextInternal::arcane_shot;
        creators["concussive shot"] = &HunterAiObjectContextInternal::concussive_shot;
        creators["distracting shot"] = &HunterAiObjectContextInternal::distracting_shot;
        creators["multi-shot"] = &HunterAiObjectContextInternal::multi_shot;
        creators["volley"] = &HunterAiObjectContextInternal::volley;
        creators["serpent sting"] = &HunterAiObjectContextInternal::serpent_sting;
        creators["serpent sting on attacker"] = &HunterAiObjectContextInternal::serpent_sting_on_attacker;
        creators["wyvern sting"] = &HunterAiObjectContextInternal::wyvern_sting;
        creators["viper sting"] = &HunterAiObjectContextInternal::viper_sting;
        creators["scorpid sting"] = &HunterAiObjectContextInternal::scorpid_sting;
        creators["hunter's mark"] = &HunterAiObjectContextInternal::hunters_mark;
        creators["mend pet"] = &HunterAiObjectContextInternal::mend_pet;
        creators["kill command"] = &HunterAiObjectContextInternal::kill_command;
        creators["revive pet"] = &HunterAiObjectContextInternal::revive_pet;
        creators["call pet 1"] = &HunterAiObjectContextInternal::call_pet;
        creators["black arrow"] = &HunterAiObjectContextInternal::black_arrow;
        creators["freezing trap"] = &HunterAiObjectContextInternal::freezing_trap;
        creators["rapid fire"] = &HunterAiObjectContextInternal::rapid_fire;
        creators["boost"] = &HunterAiObjectContextInternal::rapid_fire;
        creators["deterrence"] = &HunterAiObjectContextInternal::deterrence;
        creators["readiness"] = &HunterAiObjectContextInternal::readiness;
        creators["aspect of the hawk"] = &HunterAiObjectContextInternal::aspect_of_the_hawk;
        creators["aspect of the monkey"] = &HunterAiObjectContextInternal::aspect_of_the_monkey;
        creators["aspect of the wild"] = &HunterAiObjectContextInternal::aspect_of_the_wild;
        creators["aspect of the viper"] = &HunterAiObjectContextInternal::aspect_of_the_viper;
        creators["aspect of the pack"] = &HunterAiObjectContextInternal::aspect_of_the_pack;
        creators["aspect of the cheetah"] = &HunterAiObjectContextInternal::aspect_of_the_cheetah;
        creators["trueshot aura"] = &HunterAiObjectContextInternal::trueshot_aura;
        creators["track humanoids"] = &HunterAiObjectContextInternal::track_humanoids;
        creators["feign death"] = &HunterAiObjectContextInternal::feign_death;
        creators["wing clip"] = &HunterAiObjectContextInternal::wing_clip;
        creators["raptor strike"] = &HunterAiObjectContextInternal::raptor_strike;
        creators["mongoose bite"] = &HunterAiObjectContextInternal::mongoose_bite;
        creators["bestial wrath"] = &HunterAiObjectContextInternal::bestial_wrath;
        creators["scare beast"] = &HunterAiObjectContextInternal::scare_beast;
        creators["scare beast on cc"] = &HunterAiObjectContextInternal::scare_beast_on_cc;
        creators["aspect of the dragonhawk"] = &HunterAiObjectContextInternal::aspect_of_the_dragonhawk;
        creators["tranquilizing shot"] = &HunterAiObjectContextInternal::tranquilizing_shot;
        creators["steady shot"] = &HunterAiObjectContextInternal::steady_shot;
        creators["cobra shot"] = &HunterAiObjectContextInternal::cobra_shot;
        creators["kill shot"] = &HunterAiObjectContextInternal::kill_shot;
        creators["misdirection on main tank"] = &HunterAiObjectContextInternal::misdirection_on_main_tank;
        creators["silencing shot"] = &HunterAiObjectContextInternal::silencing_shot;
        creators["focus fire"] = &HunterAiObjectContextInternal::focus_fire;
        creators["stampede"] = &HunterAiObjectContextInternal::stampede;
        creators["dire beast"] = &HunterAiObjectContextInternal::dire_beast;
        creators["a murder of crows"] = &HunterAiObjectContextInternal::a_murder_of_crows;
        creators["glaive toss"] = &HunterAiObjectContextInternal::glaive_toss;
        creators["barrage"] = &HunterAiObjectContextInternal::barrage;
        creators["explosive trap"] = &HunterAiObjectContextInternal::explosive_trap;
        creators["fervor"] = &HunterAiObjectContextInternal::fervor;
    }

private:
    static Action* scare_beast(PlayerbotAI* botAI) { return new CastScareBeastAction(botAI); }
    static Action* scare_beast_on_cc(PlayerbotAI* botAI) { return new CastScareBeastCcAction(botAI); }
    static Action* bestial_wrath(PlayerbotAI* botAI) { return new CastBestialWrathAction(botAI); }
    static Action* feign_death(PlayerbotAI* botAI) { return new CastFeignDeathAction(botAI); }
    static Action* trueshot_aura(PlayerbotAI* botAI) { return new CastTrueshotAuraAction(botAI); }
    static Action* track_humanoids(PlayerbotAI* botAI) { return new CastBuffSpellAction(botAI, "track humanoids"); }
    static Action* auto_shot(PlayerbotAI* botAI) { return new CastAutoShotAction(botAI); }
    static Action* aimed_shot(PlayerbotAI* botAI) { return new CastAimedShotAction(botAI); }
    static Action* chimera_shot(PlayerbotAI* botAI) { return new CastChimeraShotAction(botAI); }
    static Action* explosive_shot(PlayerbotAI* botAI) { return new CastExplosiveShotAction(botAI); }
    static Action* arcane_shot(PlayerbotAI* botAI) { return new CastArcaneShotAction(botAI); }
    static Action* concussive_shot(PlayerbotAI* botAI) { return new CastConcussiveShotAction(botAI); }
    static Action* distracting_shot(PlayerbotAI* botAI) { return new CastDistractingShotAction(botAI); }
    static Action* multi_shot(PlayerbotAI* botAI) { return new CastMultiShotAction(botAI); }
    static Action* volley(PlayerbotAI* botAI) { return new CastVolleyAction(botAI); }
    static Action* serpent_sting(PlayerbotAI* botAI) { return new CastSerpentStingAction(botAI); }
    static Action* serpent_sting_on_attacker(PlayerbotAI* botAI) { return new CastSerpentStingOnAttackerAction(botAI); }
    static Action* wyvern_sting(PlayerbotAI* botAI) { return new CastWyvernStingAction(botAI); }
    static Action* viper_sting(PlayerbotAI* botAI) { return new CastViperStingAction(botAI); }
    static Action* scorpid_sting(PlayerbotAI* botAI) { return new CastScorpidStingAction(botAI); }
    static Action* hunters_mark(PlayerbotAI* botAI) { return new CastHuntersMarkAction(botAI); }
    static Action* mend_pet(PlayerbotAI* botAI) { return new CastMendPetAction(botAI); }
    static Action* kill_command(PlayerbotAI* botAI) { return new CastKillCommandAction(botAI); }
    static Action* revive_pet(PlayerbotAI* botAI) { return new CastRevivePetAction(botAI); }
    static Action* call_pet(PlayerbotAI* botAI) { return new CastCallPetAction(botAI); }
    static Action* black_arrow(PlayerbotAI* botAI) { return new CastBlackArrow(botAI); }
    static Action* freezing_trap(PlayerbotAI* botAI) { return new CastFreezingTrap(botAI); }
    static Action* rapid_fire(PlayerbotAI* botAI) { return new CastRapidFireAction(botAI); }
    static Action* deterrence(PlayerbotAI* botAI) { return new CastDeterrenceAction(botAI); }
    static Action* readiness(PlayerbotAI* botAI) { return new CastReadinessAction(botAI); }
    static Action* aspect_of_the_hawk(PlayerbotAI* botAI) { return new CastAspectOfTheHawkAction(botAI); }
    static Action* aspect_of_the_monkey(PlayerbotAI* botAI) { return new CastAspectOfTheMonkeyAction(botAI); }
    static Action* aspect_of_the_wild(PlayerbotAI* botAI) { return new CastAspectOfTheWildAction(botAI); }
    static Action* aspect_of_the_viper(PlayerbotAI* botAI) { return new CastAspectOfTheViperAction(botAI); }
    static Action* aspect_of_the_pack(PlayerbotAI* botAI) { return new CastAspectOfThePackAction(botAI); }
    static Action* aspect_of_the_cheetah(PlayerbotAI* botAI) { return new CastAspectOfTheCheetahAction(botAI); }
    static Action* wing_clip(PlayerbotAI* botAI) { return new CastWingClipAction(botAI); }
    static Action* raptor_strike(PlayerbotAI* botAI) { return new CastRaptorStrikeAction(botAI); }
    static Action* mongoose_bite(PlayerbotAI* botAI) { return new CastMongooseBiteAction(botAI); }
    static Action* aspect_of_the_dragonhawk(PlayerbotAI* ai) { return new CastAspectOfTheDragonhawkAction(ai); }
    static Action* tranquilizing_shot(PlayerbotAI* ai) { return new CastTranquilizingShotAction(ai); }
    static Action* steady_shot(PlayerbotAI* ai) { return new CastSteadyShotAction(ai); }
    static Action* cobra_shot(PlayerbotAI* ai) { return new CastCobraShotAction(ai); }
    static Action* kill_shot(PlayerbotAI* ai) { return new CastKillShotAction(ai); }
    static Action* misdirection_on_main_tank(PlayerbotAI* ai) { return new CastMisdirectionOnMainTankAction(ai); }
    static Action* silencing_shot(PlayerbotAI* ai) { return new CastSilencingShotAction(ai); }
    static Action* focus_fire(PlayerbotAI* ai) { return new CastFocusFireAction(ai); }
    static Action* stampede(PlayerbotAI* ai) { return new CastStampedeAction(ai); }
    static Action* dire_beast(PlayerbotAI* ai) { return new CastDireBeastAction(ai); }
    static Action* a_murder_of_crows(PlayerbotAI* ai) { return new CastAMurderOfCrowsAction(ai); }
    static Action* glaive_toss(PlayerbotAI* ai) { return new CastGlaiveTossAction(ai); }
    static Action* barrage(PlayerbotAI* ai) { return new CastBarrageAction(ai); }
    static Action* explosive_trap(PlayerbotAI* ai) { return new CastExplosiveTrapAction(ai); }
    static Action* fervor(PlayerbotAI* ai) { return new CastFervorAction(ai); }
    
};

HunterAiObjectContext::HunterAiObjectContext(PlayerbotAI* botAI) : AiObjectContext(botAI)
{
    strategyContexts.Add(new HunterStrategyFactoryInternal());
    strategyContexts.Add(new HunterBuffStrategyFactoryInternal());
    actionContexts.Add(new HunterAiObjectContextInternal());
    triggerContexts.Add(new HunterTriggerFactoryInternal());
}
