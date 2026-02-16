

#include "DruidAiObjectContext.h"

#include "BearTankDruidStrategy.h"
#include "BalanceDruidStrategy.h"
#include "CasterDruidStrategy.h"
#include "CatDpsDruidStrategy.h"
#include "DpsFeralDruidStrategy.h"
#include "DruidStrategy.h"
#include "DruidActions.h"
#include "DruidBearActions.h"
#include "DruidCatActions.h"
#include "DruidShapeshiftActions.h"
#include "DruidTriggers.h"
#include "GenericDruidNonCombatStrategy.h"
#include "GenericDruidStrategy.h"
#include "HealDruidStrategy.h"
#include "MeleeDruidStrategy.h"
#include "OffhealDruidCatStrategy.h"
#include "Playerbots.h"
#include "RestorationDruidStrategy.h"
#include "TankFeralDruidStrategy.h"

class DruidStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    DruidStrategyFactoryInternal()
    {
        creators["nc"] = &DruidStrategyFactoryInternal::nc;
        creators["cat aoe"] = &DruidStrategyFactoryInternal::cat_aoe;
        creators["caster aoe"] = &DruidStrategyFactoryInternal::caster_aoe;
        creators["caster debuff"] = &DruidStrategyFactoryInternal::caster_debuff;
        creators["dps debuff"] = &DruidStrategyFactoryInternal::caster_debuff;
        creators["cure"] = &DruidStrategyFactoryInternal::cure;
        creators["melee"] = &DruidStrategyFactoryInternal::melee;
        creators["buff"] = &DruidStrategyFactoryInternal::buff;
        creators["boost"] = &DruidStrategyFactoryInternal::boost;
        creators["cc"] = &DruidStrategyFactoryInternal::cc;
        creators["healer dps"] = &DruidStrategyFactoryInternal::healer_dps;
    }

private:
    static Strategy* nc(PlayerbotAI* botAI) { return new GenericDruidNonCombatStrategy(botAI); }
    static Strategy* cat_aoe(PlayerbotAI* botAI) { return new CatAoeDruidStrategy(botAI); }
    static Strategy* caster_aoe(PlayerbotAI* botAI) { return new CasterDruidAoeStrategy(botAI); }
    static Strategy* caster_debuff(PlayerbotAI* botAI) { return new CasterDruidDebuffStrategy(botAI); }
    static Strategy* cure(PlayerbotAI* botAI) { return new DruidCureStrategy(botAI); }
    static Strategy* melee(PlayerbotAI* botAI) { return new MeleeDruidStrategy(botAI); }
    static Strategy* buff(PlayerbotAI* botAI) { return new GenericDruidBuffStrategy(botAI); }
    static Strategy* boost(PlayerbotAI* botAI) { return new DruidBoostStrategy(botAI); }
    static Strategy* cc(PlayerbotAI* botAI) { return new DruidCcStrategy(botAI); }
    static Strategy* healer_dps(PlayerbotAI* botAI) { return new DruidHealerDpsStrategy(botAI); }
};

class DruidDruidStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    DruidDruidStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["bear"] = &DruidDruidStrategyFactoryInternal::bear;
        creators["tank"] = &DruidDruidStrategyFactoryInternal::bear;
        creators["guardian"] = &DruidDruidStrategyFactoryInternal::bear;
        creators["guardiantank"] = &DruidDruidStrategyFactoryInternal::bear;
        creators["cat"] = &DruidDruidStrategyFactoryInternal::cat;
        creators["caster"] = &DruidDruidStrategyFactoryInternal::caster;
        creators["dps"] = &DruidDruidStrategyFactoryInternal::cat;
        creators["heal"] = &DruidDruidStrategyFactoryInternal::heal;
        creators["balance"] = &DruidDruidStrategyFactoryInternal::balance;
        creators["restoration"] = &DruidDruidStrategyFactoryInternal::restoration;
        creators["dps feral"] = &DruidDruidStrategyFactoryInternal::dps_feral;
        creators["tank feral"] = &DruidDruidStrategyFactoryInternal::tank_feral;
        creators["offheal"] = &DruidDruidStrategyFactoryInternal::offheal;
        creators["druid"] = &DruidDruidStrategyFactoryInternal::druid;
    }

private:
    static Strategy* bear(PlayerbotAI* botAI) { return new BearTankDruidStrategy(botAI); }
    static Strategy* cat(PlayerbotAI* botAI) { return new CatDpsDruidStrategy(botAI); }
    static Strategy* caster(PlayerbotAI* botAI) { return new CasterDruidStrategy(botAI); }
    static Strategy* heal(PlayerbotAI* botAI) { return new HealDruidStrategy(botAI); }
    static Strategy* balance(PlayerbotAI* botAI) { return new BalanceDruidStrategy(botAI); }
    static Strategy* restoration(PlayerbotAI* botAI) { return new RestorationDruidStrategy(botAI); }
    static Strategy* dps_feral(PlayerbotAI* botAI) { return new DpsFeralDruidStrategy(botAI); }
    static Strategy* tank_feral(PlayerbotAI* botAI) { return new TankFeralDruidStrategy(botAI); }
    static Strategy* offheal(PlayerbotAI* botAI) { return new OffhealDruidCatStrategy(botAI); }
    static Strategy* druid(PlayerbotAI* botAI) { return new DruidStrategy(botAI); }
};

class DruidTriggerFactoryInternal : public NamedObjectContext<Trigger>
{
public:
    DruidTriggerFactoryInternal()
    {
        creators["moonfire"] = &DruidTriggerFactoryInternal::moonfire;
        creators["sunfire"] = &DruidTriggerFactoryInternal::sunfire;
        creators["thrash"] = &DruidTriggerFactoryInternal::thrash;
        creators["rake"] = &DruidTriggerFactoryInternal::rake;
        creators["rip"] = &DruidTriggerFactoryInternal::rip;
        creators["faerie fire (feral)"] = &DruidTriggerFactoryInternal::faerie_fire_feral;
        creators["savage roar"] = &DruidTriggerFactoryInternal::savage_roar;
        creators["tiger's fury"] = &DruidTriggerFactoryInternal::tigers_fury;
        creators["berserk"] = &DruidTriggerFactoryInternal::berserk;
        creators["enrage"] = &DruidTriggerFactoryInternal::enrage;
        creators["mangle (cat)"] = &DruidTriggerFactoryInternal::mangle_cat;
        creators["mangle (bear)"] = &DruidTriggerFactoryInternal::mangle_bear;
        creators["lacerate"] = &DruidTriggerFactoryInternal::lacerate;
        creators["insect swarm"] = &DruidTriggerFactoryInternal::insect_swarm;
        creators["starsurge"] = &DruidTriggerFactoryInternal::starsurge;
        creators["shooting stars"] = &DruidTriggerFactoryInternal::shooting_stars;
        creators["starfall"] = &DruidTriggerFactoryInternal::starfall;
        creators["celestial alignment"] = &DruidTriggerFactoryInternal::celestial_alignment;
        creators["astral communion"] = &DruidTriggerFactoryInternal::astral_communion;
        creators["force of nature"] = &DruidTriggerFactoryInternal::force_of_nature;
        creators["wild mushroom: detonate"] = &DruidTriggerFactoryInternal::wild_mushroom_detonate;
        creators["astral storm"] = &DruidTriggerFactoryInternal::astral_storm;
        creators["solar beam"] = &DruidTriggerFactoryInternal::solar_beam;
        creators["typhoon"] = &DruidTriggerFactoryInternal::typhoon;
        creators["ursol's vortex"] = &DruidTriggerFactoryInternal::ursols_vortex;
        creators["might of ursoc"] = &DruidTriggerFactoryInternal::might_of_ursoc;
        creators["savage defense"] = &DruidTriggerFactoryInternal::savage_defense;
        creators["ferocious bite time"] = &DruidTriggerFactoryInternal::ferocious_bite_time;
        creators["lifebloom on main tank"] = &DruidTriggerFactoryInternal::lifebloom_on_main_tank;
        creators["cenarion ward on main tank"] = &DruidTriggerFactoryInternal::cenarion_ward_on_main_tank;
        creators["genesis"] = &DruidTriggerFactoryInternal::genesis;
        creators["wild mushroom: bloom"] = &DruidTriggerFactoryInternal::wild_mushroom_bloom;
        creators["bash"] = &DruidTriggerFactoryInternal::bash;
        creators["entangling roots"] = &DruidTriggerFactoryInternal::entangling_roots;
        creators["entangling roots kite"] = &DruidTriggerFactoryInternal::entangling_roots_kite;
        creators["hibernate"] = &DruidTriggerFactoryInternal::hibernate;
    }

private:
    static Trigger* moonfire(PlayerbotAI* botAI) { return new MoonfireTrigger(botAI); }
    static Trigger* sunfire(PlayerbotAI* botAI) { return new SunfireTrigger(botAI); }
    static Trigger* thrash(PlayerbotAI* botAI) { return new ThrashTrigger(botAI); }
    static Trigger* rake(PlayerbotAI* botAI) { return new RakeTrigger(botAI); }
    static Trigger* rip(PlayerbotAI* botAI) { return new RipTrigger(botAI); }
    static Trigger* faerie_fire_feral(PlayerbotAI* botAI) { return new FaerieFireFeralTrigger(botAI); }
    static Trigger* savage_roar(PlayerbotAI* botAI) { return new SavageRoarTrigger(botAI); }
    static Trigger* tigers_fury(PlayerbotAI* botAI) { return new TigersFuryTrigger(botAI); }
    static Trigger* berserk(PlayerbotAI* botAI) { return new BerserkTrigger(botAI); }
    static Trigger* enrage(PlayerbotAI* botAI) { return new EnrageTrigger(botAI); }
    static Trigger* mangle_cat(PlayerbotAI* botAI) { return new MangleCatTrigger(botAI); }
    static Trigger* mangle_bear(PlayerbotAI* botAI) { return new MangleBearTrigger(botAI); }
    static Trigger* lacerate(PlayerbotAI* botAI) { return new LacerateTrigger(botAI); }
    static Trigger* insect_swarm(PlayerbotAI* botAI) { return new InsectSwarmTrigger(botAI); }
    static Trigger* starsurge(PlayerbotAI* botAI) { return new StarsurgeTrigger(botAI); }
    static Trigger* shooting_stars(PlayerbotAI* botAI) { return new ShootingStarsTrigger(botAI); }
    static Trigger* starfall(PlayerbotAI* botAI) { return new StarfallTrigger(botAI); }
    static Trigger* celestial_alignment(PlayerbotAI* botAI) { return new CelestialAlignmentTrigger(botAI); }
    static Trigger* astral_communion(PlayerbotAI* botAI) { return new AstralCommunionTrigger(botAI); }
    static Trigger* force_of_nature(PlayerbotAI* botAI) { return new ForceOfNatureTrigger(botAI); }
    static Trigger* wild_mushroom_detonate(PlayerbotAI* botAI) { return new WildMushroomDetonateTrigger(botAI); }
    static Trigger* astral_storm(PlayerbotAI* botAI) { return new AstralStormTrigger(botAI); }
    static Trigger* solar_beam(PlayerbotAI* botAI) { return new SolarBeamTrigger(botAI); }
    static Trigger* typhoon(PlayerbotAI* botAI) { return new TyphoonTrigger(botAI); }
    static Trigger* ursols_vortex(PlayerbotAI* botAI) { return new UrsolsVortexTrigger(botAI); }
    static Trigger* might_of_ursoc(PlayerbotAI* botAI) { return new MightOfUrsocTrigger(botAI); }
    static Trigger* savage_defense(PlayerbotAI* botAI) { return new SavageDefenseTrigger(botAI); }
    static Trigger* ferocious_bite_time(PlayerbotAI* botAI) { return new FerociousBiteTimeTrigger(botAI); }
    static Trigger* lifebloom_on_main_tank(PlayerbotAI* botAI) { return new LifebloomOnMainTankTrigger(botAI); }
    static Trigger* cenarion_ward_on_main_tank(PlayerbotAI* botAI) { return new CenarionWardOnMainTankTrigger(botAI); }
    static Trigger* genesis(PlayerbotAI* botAI) { return new GenesisTrigger(botAI); }
    static Trigger* wild_mushroom_bloom(PlayerbotAI* botAI) { return new WildMushroomBloomTrigger(botAI); }
    static Trigger* bash(PlayerbotAI* botAI) { return new HasCcTargetTrigger(botAI, "bash"); }
    static Trigger* entangling_roots(PlayerbotAI* botAI) { return new HasCcTargetTrigger(botAI, "entangling roots"); }
    static Trigger* entangling_roots_kite(PlayerbotAI* botAI) { return new EntanglingRootsKiteTrigger(botAI); }
    static Trigger* hibernate(PlayerbotAI* botAI) { return new HasCcTargetTrigger(botAI, "hibernate"); }
};

class DruidAiObjectContextInternal : public NamedObjectContext<Action>
{
public:
    DruidAiObjectContextInternal()
    {
        creators["feral charge - bear"] = &DruidAiObjectContextInternal::feral_charge_bear;
        creators["feral charge - cat"] = &DruidAiObjectContextInternal::feral_charge_cat;
        creators["swipe (bear)"] = &DruidAiObjectContextInternal::swipe_bear;
        creators["faerie fire (feral)"] = &DruidAiObjectContextInternal::faerie_fire_feral;
        creators["faerie fire"] = &DruidAiObjectContextInternal::faerie_fire;
        creators["bear form"] = &DruidAiObjectContextInternal::bear_form;
        creators["dire bear form"] = &DruidAiObjectContextInternal::dire_bear_form;
        creators["moonkin form"] = &DruidAiObjectContextInternal::moonkin_form;
        creators["cat form"] = &DruidAiObjectContextInternal::cat_form;
        creators["tree form"] = &DruidAiObjectContextInternal::tree_form;
        creators["travel form"] = &DruidAiObjectContextInternal::travel_form;
        creators["aquatic form"] = &DruidAiObjectContextInternal::aquatic_form;
        creators["caster form"] = &DruidAiObjectContextInternal::caster_form;
        creators["cancel tree form"] = &DruidAiObjectContextInternal::cancel_tree_form;
        creators["mangle (bear)"] = &DruidAiObjectContextInternal::mangle_bear;
        creators["maul"] = &DruidAiObjectContextInternal::maul;
        creators["bash"] = &DruidAiObjectContextInternal::bash;
        creators["bash on cc"] = &DruidAiObjectContextInternal::bash_on_cc;
        creators["swipe"] = &DruidAiObjectContextInternal::swipe;
        creators["growl"] = &DruidAiObjectContextInternal::growl;
        creators["demoralizing roar"] = &DruidAiObjectContextInternal::demoralizing_roar;
        creators["hibernate"] = &DruidAiObjectContextInternal::hibernate;
        creators["entangling roots"] = &DruidAiObjectContextInternal::entangling_roots;
        creators["entangling roots on cc"] = &DruidAiObjectContextInternal::entangling_roots_on_cc;
        creators["hibernate"] = &DruidAiObjectContextInternal::hibernate;
        creators["hibernate on cc"] = &DruidAiObjectContextInternal::hibernate_on_cc;
        creators["wrath"] = &DruidAiObjectContextInternal::wrath;
        creators["starfall"] = &DruidAiObjectContextInternal::starfall;
        creators["insect swarm"] = &DruidAiObjectContextInternal::insect_swarm;
        creators["moonfire"] = &DruidAiObjectContextInternal::moonfire;
        creators["sunfire"] = &DruidAiObjectContextInternal::sunfire;
        creators["starfire"] = &DruidAiObjectContextInternal::starfire;
        creators["thrash"] = &DruidAiObjectContextInternal::thrash;
        creators["thrash (bear)"] = &DruidAiObjectContextInternal::thrash_bear;
        creators["starsurge"] = &DruidAiObjectContextInternal::starsurge;
        creators["astral communion"] = &DruidAiObjectContextInternal::astral_communion;
        creators["celestial alignment"] = &DruidAiObjectContextInternal::celestial_alignment;
        creators["wild mushroom"] = &DruidAiObjectContextInternal::wild_mushroom;
        creators["wild mushroom: detonate"] = &DruidAiObjectContextInternal::wild_mushroom_detonate;
        creators["astral storm"] = &DruidAiObjectContextInternal::astral_storm;
        creators["solar beam"] = &DruidAiObjectContextInternal::solar_beam;
        creators["typhoon"] = &DruidAiObjectContextInternal::typhoon;
        creators["ursol's vortex"] = &DruidAiObjectContextInternal::ursols_vortex;
        creators["might of ursoc"] = &DruidAiObjectContextInternal::might_of_ursoc;
        creators["savage defense"] = &DruidAiObjectContextInternal::savage_defense;
        creators["lifebloom on main tank"] = &DruidAiObjectContextInternal::lifebloom_on_main_tank;
        creators["cenarion ward on main tank"] = &DruidAiObjectContextInternal::cenarion_ward_on_main_tank;
        creators["ironbark on party"] = &DruidAiObjectContextInternal::ironbark_on_party;
        creators["genesis"] = &DruidAiObjectContextInternal::genesis;
        creators["wild mushroom: bloom"] = &DruidAiObjectContextInternal::wild_mushroom_bloom;
        creators["nature's grasp"] = &DruidAiObjectContextInternal::natures_grasp;
        creators["claw"] = &DruidAiObjectContextInternal::claw;
        creators["mangle (cat)"] = &DruidAiObjectContextInternal::mangle_cat;
        creators["swipe (cat)"] = &DruidAiObjectContextInternal::swipe_cat;
        creators["rake"] = &DruidAiObjectContextInternal::rake;
        creators["rake on attacker"] = &DruidAiObjectContextInternal::rake_on_attacker;
        creators["ferocious bite"] = &DruidAiObjectContextInternal::ferocious_bite;
        creators["rip"] = &DruidAiObjectContextInternal::rip;
        creators["cower"] = &DruidAiObjectContextInternal::cower;
        creators["survival instincts"] = &DruidAiObjectContextInternal::survival_instincts;
        creators["frenzied regeneration"] = &DruidAiObjectContextInternal::frenzied_regeneration;
        creators["thorns"] = &DruidAiObjectContextInternal::thorns;
        creators["thorns on party"] = &DruidAiObjectContextInternal::thorns_on_party;
        creators["thorns on main tank"] = &DruidAiObjectContextInternal::thorns_on_main_tank;
        creators["cure poison"] = &DruidAiObjectContextInternal::cure_poison;
        creators["cure poison on party"] = &DruidAiObjectContextInternal::cure_poison_on_party;
        creators["abolish poison"] = &DruidAiObjectContextInternal::abolish_poison;
        creators["abolish poison on party"] = &DruidAiObjectContextInternal::abolish_poison_on_party;
        creators["berserk"] = &DruidAiObjectContextInternal::berserk;
        creators["tiger's fury"] = &DruidAiObjectContextInternal::tigers_fury;
        creators["savage roar"] = &DruidAiObjectContextInternal::savage_roar;
        creators["mark of the wild"] = &DruidAiObjectContextInternal::mark_of_the_wild;
        creators["mark of the wild on party"] = &DruidAiObjectContextInternal::mark_of_the_wild_on_party;
        creators["regrowth"] = &DruidAiObjectContextInternal::regrowth;
        creators["rejuvenation"] = &DruidAiObjectContextInternal::rejuvenation;
        creators["healing touch"] = &DruidAiObjectContextInternal::healing_touch;
        creators["regrowth on party"] = &DruidAiObjectContextInternal::regrowth_on_party;
        creators["rejuvenation on party"] = &DruidAiObjectContextInternal::rejuvenation_on_party;
        creators["rejuvenation on not full"] = &DruidAiObjectContextInternal::rejuvenation_on_not_full;
        creators["healing touch on party"] = &DruidAiObjectContextInternal::healing_touch_on_party;
        creators["rebirth"] = &DruidAiObjectContextInternal::rebirth;
        creators["revive"] = &DruidAiObjectContextInternal::revive;
        creators["barkskin"] = &DruidAiObjectContextInternal::barkskin;
        creators["lacerate"] = &DruidAiObjectContextInternal::lacerate;
        creators["hurricane"] = &DruidAiObjectContextInternal::hurricane;
        creators["innervate"] = &DruidAiObjectContextInternal::innervate;
        creators["tranquility"] = &DruidAiObjectContextInternal::tranquility;
        creators["bash on enemy healer"] = &DruidAiObjectContextInternal::bash_on_enemy_healer;
        creators["omen of clarity"] = &DruidAiObjectContextInternal::omen_of_clarity;
        creators["nature's swiftness"] = &DruidAiObjectContextInternal::natures_swiftness;
        creators["prowl"] = &DruidAiObjectContextInternal::prowl;
        creators["dash"] = &DruidAiObjectContextInternal::dash;
        creators["shred"] = &DruidAiObjectContextInternal::shred;
        creators["ravage"] = &DruidAiObjectContextInternal::ravage;
        creators["pounce"] = &DruidAiObjectContextInternal::pounce;
        creators["wild growth on party"] = &DruidAiObjectContextInternal::wild_growth_on_party;
        creators["swiftmend on party"] = &DruidAiObjectContextInternal::swiftmend_on_party;
        creators["nourish on party"] = &DruidAiObjectContextInternal::nourish_on_party;
        creators["remove curse on party"] = &DruidAiObjectContextInternal::remove_curse_on_party;
        creators["insect swarm on attacker"] = &DruidAiObjectContextInternal::insect_swarm_on_attacker;
        creators["moonfire on attacker"] = &DruidAiObjectContextInternal::moonfire_on_attacker;
        creators["sunfire on attacker"] = &DruidAiObjectContextInternal::sunfire_on_attacker;
        creators["enrage"] = &DruidAiObjectContextInternal::enrage;
        creators["force of nature"] = &DruidAiObjectContextInternal::force_of_nature;
    }

private:
    static Action* natures_swiftness(PlayerbotAI* botAI) { return new CastNaturesSwiftnessAction(botAI); }
    static Action* omen_of_clarity(PlayerbotAI* botAI) { return new CastOmenOfClarityAction(botAI); }
    static Action* tranquility(PlayerbotAI* botAI) { return new CastTranquilityAction(botAI); }
    static Action* feral_charge_bear(PlayerbotAI* botAI) { return new CastFeralChargeBearAction(botAI); }
    static Action* feral_charge_cat(PlayerbotAI* botAI) { return new CastFeralChargeCatAction(botAI); }
    static Action* swipe_bear(PlayerbotAI* botAI) { return new CastSwipeBearAction(botAI); }
    static Action* faerie_fire_feral(PlayerbotAI* botAI) { return new CastFaerieFireFeralAction(botAI); }
    static Action* faerie_fire(PlayerbotAI* botAI) { return new CastFaerieFireAction(botAI); }
    static Action* bear_form(PlayerbotAI* botAI) { return new CastBearFormAction(botAI); }
    static Action* dire_bear_form(PlayerbotAI* botAI) { return new CastDireBearFormAction(botAI); }
    static Action* cat_form(PlayerbotAI* botAI) { return new CastCatFormAction(botAI); }
    static Action* tree_form(PlayerbotAI* botAI) { return new CastTreeFormAction(botAI); }
    static Action* travel_form(PlayerbotAI* botAI) { return new CastTravelFormAction(botAI); }
    static Action* aquatic_form(PlayerbotAI* botAI) { return new CastAquaticFormAction(botAI); }
    static Action* caster_form(PlayerbotAI* botAI) { return new CastCasterFormAction(botAI); }
    static Action* cancel_tree_form(PlayerbotAI* botAI) { return new CastCancelTreeFormAction(botAI); }
    static Action* mangle_bear(PlayerbotAI* botAI) { return new CastMangleBearAction(botAI); }
    static Action* maul(PlayerbotAI* botAI) { return new CastMaulAction(botAI); }
    static Action* bash(PlayerbotAI* botAI) { return new CastBashAction(botAI); }
    static Action* bash_on_cc(PlayerbotAI* botAI)
    {
        return new CastCrowdControlSpellAction(botAI, "bash");
    }
    static Action* swipe(PlayerbotAI* botAI) { return new CastSwipeAction(botAI); }
    static Action* growl(PlayerbotAI* botAI) { return new CastGrowlAction(botAI); }
    static Action* demoralizing_roar(PlayerbotAI* botAI) { return new CastDemoralizingRoarAction(botAI); }
    static Action* moonkin_form(PlayerbotAI* botAI) { return new CastMoonkinFormAction(botAI); }
    static Action* hibernate(PlayerbotAI* botAI) { return new CastHibernateAction(botAI); }
    static Action* entangling_roots(PlayerbotAI* botAI) { return new CastEntanglingRootsAction(botAI); }
    static Action* hibernate_on_cc(PlayerbotAI* botAI) { return new CastHibernateCcAction(botAI); }
    static Action* entangling_roots_on_cc(PlayerbotAI* botAI) { return new CastEntanglingRootsCcAction(botAI); }
    static Action* wrath(PlayerbotAI* botAI) { return new CastWrathAction(botAI); }
    static Action* starfall(PlayerbotAI* botAI) { return new CastStarfallAction(botAI); }
    static Action* insect_swarm(PlayerbotAI* botAI) { return new CastInsectSwarmAction(botAI); }
    static Action* moonfire(PlayerbotAI* botAI) { return new CastMoonfireAction(botAI); }
    static Action* sunfire(PlayerbotAI* botAI) { return new CastSunfireAction(botAI); }
    static Action* starfire(PlayerbotAI* botAI) { return new CastStarfireAction(botAI); }
    static Action* thrash(PlayerbotAI* botAI) { return new CastThrashAction(botAI); }
    static Action* thrash_bear(PlayerbotAI* botAI) { return new CastThrashBearAction(botAI); }
    static Action* starsurge(PlayerbotAI* botAI) { return new CastStarsurgeAction(botAI); }
    static Action* astral_communion(PlayerbotAI* botAI) { return new CastAstralCommunionAction(botAI); }
    static Action* celestial_alignment(PlayerbotAI* botAI) { return new CastCelestialAlignmentAction(botAI); }
    static Action* wild_mushroom(PlayerbotAI* botAI) { return new CastWildMushroomAction(botAI); }
    static Action* wild_mushroom_detonate(PlayerbotAI* botAI) { return new CastWildMushroomDetonateAction(botAI); }
    static Action* astral_storm(PlayerbotAI* botAI) { return new CastAstralStormAction(botAI); }
    static Action* solar_beam(PlayerbotAI* botAI) { return new CastSolarBeamAction(botAI); }
    static Action* typhoon(PlayerbotAI* botAI) { return new CastTyphoonAction(botAI); }
    static Action* ursols_vortex(PlayerbotAI* botAI) { return new CastUrsolsVortexAction(botAI); }
    static Action* might_of_ursoc(PlayerbotAI* botAI) { return new CastMightOfUrsocAction(botAI); }
    static Action* savage_defense(PlayerbotAI* botAI) { return new CastSavageDefenseAction(botAI); }
    static Action* lifebloom_on_main_tank(PlayerbotAI* botAI) { return new CastLifebloomOnMainTankAction(botAI); }
    static Action* cenarion_ward_on_main_tank(PlayerbotAI* botAI) { return new CastCenarionWardOnMainTankAction(botAI); }
    static Action* ironbark_on_party(PlayerbotAI* botAI) { return new CastIronbarkOnPartyAction(botAI); }
    static Action* genesis(PlayerbotAI* botAI) { return new CastGenesisAction(botAI); }
    static Action* wild_mushroom_bloom(PlayerbotAI* botAI) { return new CastWildMushroomBloomAction(botAI); }
    static Action* natures_grasp(PlayerbotAI* botAI) { return new CastNaturesGraspAction(botAI); }
    static Action* claw(PlayerbotAI* botAI) { return new CastClawAction(botAI); }
    static Action* mangle_cat(PlayerbotAI* botAI) { return new CastMangleCatAction(botAI); }
    static Action* swipe_cat(PlayerbotAI* botAI) { return new CastSwipeCatAction(botAI); }
    static Action* rake(PlayerbotAI* botAI) { return new CastRakeAction(botAI); }
    static Action* rake_on_attacker(PlayerbotAI* botAI) { return new CastRakeOnMeleeAttackersAction(botAI); }
    static Action* ferocious_bite(PlayerbotAI* botAI) { return new CastFerociousBiteAction(botAI); }
    static Action* rip(PlayerbotAI* botAI) { return new CastRipAction(botAI); }
    static Action* cower(PlayerbotAI* botAI) { return new CastCowerAction(botAI); }
    static Action* survival_instincts(PlayerbotAI* botAI) { return new CastSurvivalInstinctsAction(botAI); }
    static Action* frenzied_regeneration(PlayerbotAI* botAI) { return new CastFrenziedRegenerationAction(botAI); }
    static Action* thorns(PlayerbotAI* botAI) { return new CastThornsAction(botAI); }
    static Action* thorns_on_party(PlayerbotAI* botAI) { return new CastThornsOnPartyAction(botAI); }
    static Action* thorns_on_main_tank(PlayerbotAI* botAI) { return new CastThornsOnMainTankAction(botAI); }
    static Action* cure_poison(PlayerbotAI* botAI) { return new CastCurePoisonAction(botAI); }
    static Action* cure_poison_on_party(PlayerbotAI* botAI) { return new CastCurePoisonOnPartyAction(botAI); }
    static Action* abolish_poison(PlayerbotAI* botAI) { return new CastAbolishPoisonAction(botAI); }
    static Action* abolish_poison_on_party(PlayerbotAI* botAI) { return new CastAbolishPoisonOnPartyAction(botAI); }
    static Action* berserk(PlayerbotAI* botAI) { return new CastBerserkAction(botAI); }
    static Action* tigers_fury(PlayerbotAI* botAI) { return new CastTigersFuryAction(botAI); }
    static Action* savage_roar(PlayerbotAI* botAI) { return new CastSavageRoarAction(botAI); }
    static Action* mark_of_the_wild(PlayerbotAI* botAI) { return new CastMarkOfTheWildAction(botAI); }
    static Action* mark_of_the_wild_on_party(PlayerbotAI* botAI) { return new CastMarkOfTheWildOnPartyAction(botAI); }
    static Action* regrowth(PlayerbotAI* botAI) { return new CastRegrowthAction(botAI); }
    static Action* rejuvenation(PlayerbotAI* botAI) { return new CastRejuvenationAction(botAI); }
    static Action* healing_touch(PlayerbotAI* botAI) { return new CastHealingTouchAction(botAI); }
    static Action* regrowth_on_party(PlayerbotAI* botAI) { return new CastRegrowthOnPartyAction(botAI); }
    static Action* rejuvenation_on_party(PlayerbotAI* botAI) { return new CastRejuvenationOnPartyAction(botAI); }
    static Action* rejuvenation_on_not_full(PlayerbotAI* botAI) { return new CastRejuvenationOnNotFullAction(botAI); }
    static Action* healing_touch_on_party(PlayerbotAI* botAI) { return new CastHealingTouchOnPartyAction(botAI); }
    static Action* rebirth(PlayerbotAI* botAI) { return new CastRebirthAction(botAI); }
    static Action* revive(PlayerbotAI* botAI) { return new CastReviveAction(botAI); }
    static Action* barkskin(PlayerbotAI* botAI) { return new CastBarkskinAction(botAI); }
    static Action* lacerate(PlayerbotAI* botAI) { return new CastLacerateAction(botAI); }
    static Action* hurricane(PlayerbotAI* botAI) { return new CastHurricaneAction(botAI); }
    static Action* innervate(PlayerbotAI* botAI) { return new CastInnervateAction(botAI); }
    static Action* bash_on_enemy_healer(PlayerbotAI* botAI) { return new CastBashOnEnemyHealerAction(botAI); }
    static Action* ravage(PlayerbotAI* botAI) { return new CastRavageAction(botAI); }
    static Action* pounce(PlayerbotAI* botAI) { return new CastPounceAction(botAI); }
    static Action* prowl(PlayerbotAI* botAI) { return new CastProwlAction(botAI); }
    static Action* dash(PlayerbotAI* botAI) { return new CastDashAction(botAI); }
    static Action* shred(PlayerbotAI* botAI) { return new CastShredAction(botAI); }
    static Action* wild_growth_on_party(PlayerbotAI* ai) { return new CastWildGrowthOnPartyAction(ai); }
    static Action* swiftmend_on_party(PlayerbotAI* ai) { return new CastPartySwiftmendAction(ai); }
    static Action* nourish_on_party(PlayerbotAI* ai) { return new CastPartyNourishAction(ai); }
    static Action* remove_curse_on_party(PlayerbotAI* ai) { return new CastDruidRemoveCurseOnPartyAction(ai); }
    static Action* insect_swarm_on_attacker(PlayerbotAI* ai) { return new CastInsectSwarmOnAttackerAction(ai); }
    static Action* moonfire_on_attacker(PlayerbotAI* ai) { return new CastMoonfireOnAttackerAction(ai); }
    static Action* sunfire_on_attacker(PlayerbotAI* ai) { return new CastSunfireOnAttackerAction(ai); }
    static Action* enrage(PlayerbotAI* ai) { return new CastEnrageAction(ai); }
    static Action* force_of_nature(PlayerbotAI* ai) { return new CastForceOfNatureAction(ai); }
};

DruidAiObjectContext::DruidAiObjectContext(PlayerbotAI* botAI) : AiObjectContext(botAI)
{
    strategyContexts.Add(new DruidStrategyFactoryInternal());
    strategyContexts.Add(new DruidDruidStrategyFactoryInternal());
    actionContexts.Add(new DruidAiObjectContextInternal());
    triggerContexts.Add(new DruidTriggerFactoryInternal());
}
