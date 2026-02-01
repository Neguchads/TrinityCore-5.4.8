

#include "RogueAiObjectContext.h"

#include "AssassinationRogueStrategy.h"
#include "CombatRogueStrategy.h"
#include "DpsRogueStrategy.h"
#include "GenericRogueNonCombatStrategy.h"
#include "NamedObjectContext.h"
#include "Playerbots.h"
#include "RogueActions.h"
#include "RogueComboActions.h"
#include "RogueFinishingActions.h"
#include "RogueOpeningActions.h"
#include "RogueStrategy.h"
#include "SubtletyRogueStrategy.h"
#include "RogueTriggers.h"

class RogueStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    RogueStrategyFactoryInternal()
    {
        creators["nc"] = &RogueStrategyFactoryInternal::nc;
        //creators["pull"] = &RogueStrategyFactoryInternal::pull;
        creators["aoe"] = &RogueStrategyFactoryInternal::aoe;
        creators["boost"] = &RogueStrategyFactoryInternal::boost;
        creators["stealthed"] = &RogueStrategyFactoryInternal::stealthed;
        creators["stealth"] = &RogueStrategyFactoryInternal::stealth;
        creators["cc"] = &RogueStrategyFactoryInternal::cc;
        creators["rogue"] = &RogueStrategyFactoryInternal::rogue;
    }

private:
    static Strategy* boost(PlayerbotAI* botAI) { return new RogueBoostStrategy(botAI); }
    static Strategy* aoe(PlayerbotAI* botAI) { return new RogueAoeStrategy(botAI); }
    static Strategy* nc(PlayerbotAI* botAI) { return new GenericRogueNonCombatStrategy(botAI); }
    //static Strategy* pull(PlayerbotAI* botAI) { return new PullStrategy(botAI, "shoot"); }
    static Strategy* stealthed(PlayerbotAI* botAI) { return new StealthedRogueStrategy(botAI); }
    static Strategy* stealth(PlayerbotAI* botAI) { return new StealthStrategy(botAI); }
    static Strategy* cc(PlayerbotAI* botAI) { return new RogueCcStrategy(botAI); }
    static Strategy* rogue(PlayerbotAI* botAI) { return new RogueStrategy(botAI); }
};

class RogueCombatStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    RogueCombatStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["dps"] = &RogueCombatStrategyFactoryInternal::dps;
        creators["melee"] = &RogueCombatStrategyFactoryInternal::melee;
        creators["combat"] = &RogueCombatStrategyFactoryInternal::combat;
        creators["subtlety"] = &RogueCombatStrategyFactoryInternal::subtlety;
    }

private:
    static Strategy* dps(PlayerbotAI* botAI) { return new DpsRogueStrategy(botAI); }
    static Strategy* melee(PlayerbotAI* botAI) { return new AssassinationRogueStrategy(botAI); }
    static Strategy* combat(PlayerbotAI* botAI) { return new CombatRogueStrategy(botAI); }
    static Strategy* subtlety(PlayerbotAI* botAI) { return new SubtletyRogueStrategy(botAI); }
};

class RogueTriggerFactoryInternal : public NamedObjectContext<Trigger>
{
public:
    RogueTriggerFactoryInternal()
    {
        creators["slice and dice"] = &RogueTriggerFactoryInternal::slice_and_dice;
        creators["rupture"] = &RogueTriggerFactoryInternal::rupture;
        creators["revealing strike"] = &RogueTriggerFactoryInternal::revealing_strike;
        creators["hemorrhage"] = &RogueTriggerFactoryInternal::hemorrhage;
        creators["blade flurry"] = &RogueTriggerFactoryInternal::blade_flurry;
        creators["adrenaline rush"] = &RogueTriggerFactoryInternal::adrenaline_rush;
        creators["shadow blades"] = &RogueTriggerFactoryInternal::shadow_blades;
        creators["shadow dance"] = &RogueTriggerFactoryInternal::shadow_dance;
        creators["vendetta"] = &RogueTriggerFactoryInternal::vendetta;
        creators["blindside"] = &RogueTriggerFactoryInternal::blindside;
    }

private:
    static Trigger* slice_and_dice(PlayerbotAI* botAI) { return new SliceAndDiceTrigger(botAI); }
    static Trigger* rupture(PlayerbotAI* botAI) { return new RuptureTrigger(botAI); }
    static Trigger* revealing_strike(PlayerbotAI* botAI) { return new RevealingStrikeTrigger(botAI); }
    static Trigger* hemorrhage(PlayerbotAI* botAI) { return new HemorrhageTrigger(botAI); }
    static Trigger* blade_flurry(PlayerbotAI* botAI) { return new BladeFlurryTrigger(botAI); }
    static Trigger* adrenaline_rush(PlayerbotAI* botAI) { return new AdrenalineRushTrigger(botAI); }
    static Trigger* shadow_blades(PlayerbotAI* botAI) { return new ShadowBladesTrigger(botAI); }
    static Trigger* shadow_dance(PlayerbotAI* botAI) { return new ShadowDanceTrigger(botAI); }
    static Trigger* vendetta(PlayerbotAI* botAI) { return new VendettaTrigger(botAI); }
    static Trigger* blindside(PlayerbotAI* botAI) { return new BlindsideTrigger(botAI); }

};

class RogueAiObjectContextInternal : public NamedObjectContext<Action>
{
public:
    RogueAiObjectContextInternal()
    {
        creators["riposte"] = &RogueAiObjectContextInternal::riposte;
        creators["mutilate"] = &RogueAiObjectContextInternal::mutilate;
        creators["sinister strike"] = &RogueAiObjectContextInternal::sinister_strike;
        creators["dispatch"] = &RogueAiObjectContextInternal::dispatch;
        creators["revealing strike"] = &RogueAiObjectContextInternal::revealing_strike;
        creators["hemorrhage"] = &RogueAiObjectContextInternal::hemorrhage;
        creators["crimson tempest"] = &RogueAiObjectContextInternal::crimson_tempest;
        creators["shiv"] = &RogueAiObjectContextInternal::shiv;
        creators["gouge"] = &RogueAiObjectContextInternal::gouge;
        creators["kidney shot"] = &RogueAiObjectContextInternal::kidney_shot;
        creators["rupture"] = &RogueAiObjectContextInternal::rupture;
        creators["slice and dice"] = &RogueAiObjectContextInternal::slice_and_dice;
        creators["hunger for blood"] = &RogueAiObjectContextInternal::hunger_for_blood;
        creators["eviscerate"] = &RogueAiObjectContextInternal::eviscerate;
        creators["vanish"] = &RogueAiObjectContextInternal::vanish;
        creators["evasion"] = &RogueAiObjectContextInternal::evasion;
        creators["cloak of shadows"] = &RogueAiObjectContextInternal::cloak_of_shadows;
        creators["kick"] = &RogueAiObjectContextInternal::kick;
        creators["feint"] = &RogueAiObjectContextInternal::feint;
        creators["backstab"] = &RogueAiObjectContextInternal::backstab;
        creators["expose armor"] = &RogueAiObjectContextInternal::expose_armor;
        creators["kick on enemy healer"] = &RogueAiObjectContextInternal::kick_on_enemy_healer;
        creators["blade flurry"] = &RogueAiObjectContextInternal::blade_flurry;
        creators["adrenaline rush"] = &RogueAiObjectContextInternal::adrenaline_rush;
        creators["shadow blades"] = &RogueAiObjectContextInternal::shadow_blades;
        creators["vendetta"] = &RogueAiObjectContextInternal::vendetta;
        creators["shadow dance"] = &RogueAiObjectContextInternal::shadow_dance;
        creators["preparation"] = &RogueAiObjectContextInternal::preparation;
        creators["premeditation"] = &RogueAiObjectContextInternal::premeditation;
        creators["ambush"] = &RogueAiObjectContextInternal::ambush;
        creators["stealth"] = &RogueAiObjectContextInternal::stealth;
        creators["sprint"] = &RogueAiObjectContextInternal::sprint;
        creators["garrote"] = &RogueAiObjectContextInternal::garrote;
        creators["cheap shot"] = &RogueAiObjectContextInternal::cheap_shot;
        creators["blind"] = &RogueAiObjectContextInternal::blind;
        creators["unstealth"] = &RogueAiObjectContextInternal::unstealth;
        creators["sap"] = &RogueAiObjectContextInternal::sap;
        creators["check stealth"] = &RogueAiObjectContextInternal::check_stealth;
        creators["envenom"] = &RogueAiObjectContextInternal::envenom;
        creators["tricks of the trade on main tank"] = &RogueAiObjectContextInternal::tricks_of_the_trade_on_main_tank;
        creators["fan of knives"] = &RogueAiObjectContextInternal::fan_of_knives;
        creators["killing spree"] = &RogueAiObjectContextInternal::killing_spree;
    }

private:
    static Action* adrenaline_rush(PlayerbotAI* botAI) { return new CastAdrenalineRushAction(botAI); }
    static Action* blade_flurry(PlayerbotAI* botAI) { return new CastBladeFlurryAction(botAI); }
    static Action* riposte(PlayerbotAI* botAI) { return new CastRiposteAction(botAI); }
    static Action* mutilate(PlayerbotAI* botAI) { return new CastMutilateAction(botAI); }
    static Action* sinister_strike(PlayerbotAI* botAI) { return new CastSinisterStrikeAction(botAI); }
    static Action* dispatch(PlayerbotAI* botAI) { return new CastDispatchAction(botAI); }
    static Action* revealing_strike(PlayerbotAI* botAI) { return new CastRevealingStrikeAction(botAI); }
    static Action* hemorrhage(PlayerbotAI* botAI) { return new CastHemorrhageAction(botAI); }
    static Action* crimson_tempest(PlayerbotAI* botAI) { return new CastCrimsonTempestAction(botAI); }
    static Action* shiv(PlayerbotAI* botAI) { return new CastShivAction(botAI); }
    static Action* gouge(PlayerbotAI* botAI) { return new CastGougeAction(botAI); }
    static Action* kidney_shot(PlayerbotAI* botAI) { return new CastKidneyShotAction(botAI); }
    static Action* rupture(PlayerbotAI* botAI) { return new CastRuptureAction(botAI); }
    static Action* slice_and_dice(PlayerbotAI* botAI) { return new CastSliceAndDiceAction(botAI); }
    static Action* hunger_for_blood(PlayerbotAI* botAI) { return new CastHungerForBloodAction(botAI); }
    static Action* eviscerate(PlayerbotAI* botAI) { return new CastEviscerateAction(botAI); }
    static Action* vanish(PlayerbotAI* botAI) { return new CastVanishAction(botAI); }
    static Action* evasion(PlayerbotAI* botAI) { return new CastEvasionAction(botAI); }
    static Action* cloak_of_shadows(PlayerbotAI* botAI) { return new CastCloakOfShadowsAction(botAI); }
    static Action* kick(PlayerbotAI* botAI) { return new CastKickAction(botAI); }
    static Action* feint(PlayerbotAI* botAI) { return new CastFeintAction(botAI); }
    static Action* backstab(PlayerbotAI* botAI) { return new CastBackstabAction(botAI); }
    static Action* expose_armor(PlayerbotAI* botAI) { return new CastExposeArmorAction(botAI); }
    static Action* kick_on_enemy_healer(PlayerbotAI* botAI) { return new CastKickOnEnemyHealerAction(botAI); }
    static Action* shadow_blades(PlayerbotAI* botAI) { return new CastShadowBladesAction(botAI); }
    static Action* vendetta(PlayerbotAI* botAI) { return new CastVendettaAction(botAI); }
    static Action* shadow_dance(PlayerbotAI* botAI) { return new CastShadowDanceAction(botAI); }
    static Action* preparation(PlayerbotAI* botAI) { return new CastPreparationAction(botAI); }
    static Action* premeditation(PlayerbotAI* botAI) { return new CastPremeditationAction(botAI); }
    static Action* ambush(PlayerbotAI* botAI) { return new CastAmbushAction(botAI); }
    static Action* stealth(PlayerbotAI* botAI) { return new CastStealthAction(botAI); }
    static Action* sprint(PlayerbotAI* botAI) { return new CastSprintAction(botAI); }
    static Action* garrote(PlayerbotAI* botAI) { return new CastGarroteAction(botAI); }
    static Action* cheap_shot(PlayerbotAI* botAI) { return new CastCheapShotAction(botAI); }
    static Action* blind(PlayerbotAI* botAI) { return new CastBlindAction(botAI); }
    static Action* check_stealth(PlayerbotAI* botAI) { return new CheckStealthAction(botAI); }
    static Action* sap(PlayerbotAI* botAI) { return new CastSapAction(botAI); }
    static Action* unstealth(PlayerbotAI* botAI) { return new UnstealthAction(botAI); }
    static Action* envenom(PlayerbotAI* ai) { return new EnvenomAction(ai); }
    static Action* tricks_of_the_trade_on_main_tank(PlayerbotAI* ai)
    {
        return new CastTricksOfTheTradeOnMainTankAction(ai);
    }

    static Action* fan_of_knives(PlayerbotAI* ai) { return new FanOfKnivesAction(ai); }
    static Action* killing_spree(PlayerbotAI* ai) { return new CastKillingSpreeAction(ai); }
};

RogueAiObjectContext::RogueAiObjectContext(PlayerbotAI* botAI) : AiObjectContext(botAI)
{
    strategyContexts.Add(new RogueStrategyFactoryInternal());
    strategyContexts.Add(new RogueCombatStrategyFactoryInternal());
    actionContexts.Add(new RogueAiObjectContextInternal());
    triggerContexts.Add(new RogueTriggerFactoryInternal());
}
