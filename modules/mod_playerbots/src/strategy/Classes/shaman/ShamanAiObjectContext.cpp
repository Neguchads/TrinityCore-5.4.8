

#include "ShamanAiObjectContext.h"

#include "CasterShamanStrategy.h"
#include "ElementalShamanStrategy.h"
#include "EnhancementShamanStrategy.h"
#include "GenericShamanStrategy.h"
#include "HealShamanStrategy.h"
#include "MeleeShamanStrategy.h"
#include "NamedObjectContext.h"
#include "Playerbots.h"
#include "RestoShamanStrategy.h"
#include "RestorationShamanStrategy.h"
#include "ShamanActions.h"
#include "ShamanNonCombatStrategy.h"
#include "ShamanStrategy.h"
#include "ShamanTriggers.h"
#include "TotemsShamanStrategy.h"

class ShamanStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    ShamanStrategyFactoryInternal()
    {
        creators["nc"] = &ShamanStrategyFactoryInternal::nc;
        creators["totems"] = &ShamanStrategyFactoryInternal::totems;
        creators["melee aoe"] = &ShamanStrategyFactoryInternal::melee_aoe;
        creators["caster aoe"] = &ShamanStrategyFactoryInternal::caster_aoe;
        creators["cure"] = &ShamanStrategyFactoryInternal::cure;
        creators["healer dps"] = &ShamanStrategyFactoryInternal::healer_dps;
        creators["shaman"] = &ShamanStrategyFactoryInternal::shaman;
    }

private:
    static Strategy* nc(PlayerbotAI* botAI) { return new ShamanNonCombatStrategy(botAI); }
    static Strategy* totems(PlayerbotAI* botAI) { return new TotemsShamanStrategy(botAI); }
    static Strategy* melee_aoe(PlayerbotAI* botAI) { return new MeleeAoeShamanStrategy(botAI); }
    static Strategy* caster_aoe(PlayerbotAI* botAI) { return new CasterAoeShamanStrategy(botAI); }
    static Strategy* cure(PlayerbotAI* botAI) { return new ShamanCureStrategy(botAI); }
    static Strategy* healer_dps(PlayerbotAI* botAI) { return new ShamanHealerDpsStrategy(botAI); }
    static Strategy* shaman(PlayerbotAI* botAI) { return new ShamanStrategy(botAI); }
};

class ShamanBuffStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    ShamanBuffStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["bmana"] = &ShamanBuffStrategyFactoryInternal::bmana;
        creators["bdps"] = &ShamanBuffStrategyFactoryInternal::bdps;
    }

private:
    static Strategy* bmana(PlayerbotAI* botAI) { return new ShamanBuffManaStrategy(botAI); }
    static Strategy* bdps(PlayerbotAI* botAI) { return new ShamanBuffDpsStrategy(botAI); }
};

class ShamanCombatStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    ShamanCombatStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["heal"] = &ShamanCombatStrategyFactoryInternal::heal;
        creators["melee"] = &ShamanCombatStrategyFactoryInternal::dps;
        creators["dps"] = &ShamanCombatStrategyFactoryInternal::dps;
        creators["caster"] = &ShamanCombatStrategyFactoryInternal::caster;
        creators["elemental"] = &ShamanCombatStrategyFactoryInternal::elemental;
        creators["enhancement"] = &ShamanCombatStrategyFactoryInternal::enhancement;
        creators["restoration"] = &ShamanCombatStrategyFactoryInternal::restoration;
        creators["resto"] = &ShamanCombatStrategyFactoryInternal::resto;
    }

private:
    static Strategy* heal(PlayerbotAI* botAI) { return new HealShamanStrategy(botAI); }
    static Strategy* dps(PlayerbotAI* botAI) { return new MeleeShamanStrategy(botAI); }
    static Strategy* caster(PlayerbotAI* botAI) { return new CasterShamanStrategy(botAI); }
    static Strategy* elemental(PlayerbotAI* botAI) { return new ElementalShamanStrategy(botAI); }
    static Strategy* enhancement(PlayerbotAI* botAI) { return new EnhancementShamanStrategy(botAI); }
    static Strategy* restoration(PlayerbotAI* botAI) { return new RestorationShamanStrategy(botAI); }
    static Strategy* resto(PlayerbotAI* botAI) { return new RestoShamanStrategy(botAI); }
};

class ShamanATriggerFactoryInternal : public NamedObjectContext<Trigger>
{
public:
    ShamanATriggerFactoryInternal()
    {
        creators["flame shock"] = &ShamanATriggerFactoryInternal::flame_shock;
        creators["lightning shield"] = &ShamanATriggerFactoryInternal::lightning_shield;
        creators["lightning shield 6"] = &ShamanATriggerFactoryInternal::lightning_shield_6;
        creators["maelstrom weapon 4"] = &ShamanATriggerFactoryInternal::maelstrom_weapon_4;
        creators["maelstrom weapon 5"] = &ShamanATriggerFactoryInternal::maelstrom_weapon_5;
        creators["lava surge"] = &ShamanATriggerFactoryInternal::lava_surge;
        creators["elemental mastery"] = &ShamanATriggerFactoryInternal::elemental_mastery;
        creators["ascendance"] = &ShamanATriggerFactoryInternal::ascendance;
        creators["elemental blast"] = &ShamanATriggerFactoryInternal::elemental_blast;
        creators["unleash elements"] = &ShamanATriggerFactoryInternal::unleash_elements;
        creators["fire elemental totem"] = &ShamanATriggerFactoryInternal::fire_elemental_totem;
        creators["earth elemental totem"] = &ShamanATriggerFactoryInternal::earth_elemental_totem;
        creators["healing tide totem"] = &ShamanATriggerFactoryInternal::healing_tide_totem;
        creators["spirit link totem"] = &ShamanATriggerFactoryInternal::spirit_link_totem;
        creators["no fire totem"] = &ShamanATriggerFactoryInternal::no_fire_totem;
        creators["no air totem"] = &ShamanATriggerFactoryInternal::no_air_totem;
        creators["no water totem"] = &ShamanATriggerFactoryInternal::no_water_totem;
        creators["no earth totem"] = &ShamanATriggerFactoryInternal::no_earth_totem;
        creators["strength of earth totem"] = &ShamanATriggerFactoryInternal::strength_of_earth_totem;
        creators["main hand weapon no imbue"] = &ShamanATriggerFactoryInternal::main_hand_weapon_no_imbue;
        creators["off hand weapon no imbue"] = &ShamanATriggerFactoryInternal::off_hand_weapon_no_imbue;
        creators["wind shear"] = &ShamanATriggerFactoryInternal::wind_shear;
        creators["wind shear on enemy healer"] = &ShamanATriggerFactoryInternal::wind_shear_on_enemy_healer;
        creators["purge"] = &ShamanATriggerFactoryInternal::purge;
        creators["cure poison"] = &ShamanATriggerFactoryInternal::cure_poison;
        creators["party member cure poison"] = &ShamanATriggerFactoryInternal::cure_poison_on_party;
        creators["cure disease"] = &ShamanATriggerFactoryInternal::cure_disease;
        creators["party member cure disease"] = &ShamanATriggerFactoryInternal::cure_disease_on_party;
        creators["cleanse spirit poison"] = &ShamanATriggerFactoryInternal::cleanse_spirit_poison;
        creators["party member cleanse spirit poison"] = &ShamanATriggerFactoryInternal::cleanse_spirit_poison_on_party;
        creators["cleanse spirit disease"] = &ShamanATriggerFactoryInternal::cleanse_spirit_disease;
        creators["party member cleanse spirit disease"] = &ShamanATriggerFactoryInternal::cleanse_spirit_disease_on_party;
        creators["cleanse spirit curse"] = &ShamanATriggerFactoryInternal::cleanse_spirit_curse;
        creators["party member cleanse spirit curse"] = &ShamanATriggerFactoryInternal::cleanse_spirit_curse_on_party;
        creators["heroism"] = &ShamanATriggerFactoryInternal::heroism;
        creators["bloodlust"] = &ShamanATriggerFactoryInternal::bloodlust;
        creators["earth shield on main tank"] = &ShamanATriggerFactoryInternal::earth_shield_on_main_tank;
        creators["water shield"] = &ShamanATriggerFactoryInternal::water_shield;
    }

private:
    static Trigger* flame_shock(PlayerbotAI* botAI) { return new FlameShockTrigger(botAI); }
    static Trigger* lightning_shield(PlayerbotAI* botAI) { return new LightningShieldTrigger(botAI); }
    static Trigger* lightning_shield_6(PlayerbotAI* botAI) { return new LightningShieldChargesTrigger(botAI, 6); }
    static Trigger* maelstrom_weapon_4(PlayerbotAI* botAI) { return new MaelstromWeaponTrigger(botAI, 4); }
    static Trigger* maelstrom_weapon_5(PlayerbotAI* botAI) { return new MaelstromWeaponTrigger(botAI, 5); }
    static Trigger* lava_surge(PlayerbotAI* botAI) { return new LavaSurgeTrigger(botAI); }
    static Trigger* elemental_mastery(PlayerbotAI* botAI) { return new ElementalMasteryTrigger(botAI); }
    static Trigger* ascendance(PlayerbotAI* botAI) { return new AscendanceTrigger(botAI); }
    static Trigger* elemental_blast(PlayerbotAI* botAI) { return new ElementalBlastTrigger(botAI); }
    static Trigger* unleash_elements(PlayerbotAI* botAI) { return new UnleashElementsTrigger(botAI); }
    static Trigger* fire_elemental_totem(PlayerbotAI* botAI) { return new FireElementalTotemTrigger(botAI); }
    static Trigger* earth_elemental_totem(PlayerbotAI* botAI) { return new EarthElementalTotemTrigger(botAI); }
    static Trigger* healing_tide_totem(PlayerbotAI* botAI) { return new HealingTideTotemTrigger(botAI); }
    static Trigger* spirit_link_totem(PlayerbotAI* botAI) { return new SpiritLinkTotemTrigger(botAI); }
    static Trigger* no_fire_totem(PlayerbotAI* botAI) { return new NoFireTotemTrigger(botAI); }
    static Trigger* no_air_totem(PlayerbotAI* botAI) { return new NoAirTotemTrigger(botAI); }
    static Trigger* no_water_totem(PlayerbotAI* botAI) { return new NoWaterTotemTrigger(botAI); }
    static Trigger* no_earth_totem(PlayerbotAI* botAI) { return new NoEarthTotemTrigger(botAI); }
    static Trigger* strength_of_earth_totem(PlayerbotAI* botAI)
    {
        return new TotemMissingTrigger(botAI, "strength of earth totem");
    }
    static Trigger* main_hand_weapon_no_imbue(PlayerbotAI* botAI) { return new MainHandWeaponNoImbueTrigger(botAI); }
    static Trigger* off_hand_weapon_no_imbue(PlayerbotAI* botAI) { return new OffHandWeaponNoImbueTrigger(botAI); }
    static Trigger* wind_shear(PlayerbotAI* botAI) { return new WindShearTrigger(botAI); }
    static Trigger* wind_shear_on_enemy_healer(PlayerbotAI* botAI) { return new WindShearEnemyHealerTrigger(botAI); }
    static Trigger* purge(PlayerbotAI* botAI) { return new PurgeTrigger(botAI); }
    static Trigger* cure_poison(PlayerbotAI* botAI) { return new CurePoisonTrigger(botAI); }
    static Trigger* cure_poison_on_party(PlayerbotAI* botAI) { return new CurePoisonOnPartyTrigger(botAI); }
    static Trigger* cure_disease(PlayerbotAI* botAI) { return new CureDiseaseTrigger(botAI); }
    static Trigger* cure_disease_on_party(PlayerbotAI* botAI) { return new CureDiseaseOnPartyTrigger(botAI); }
    static Trigger* cleanse_spirit_poison(PlayerbotAI* botAI) { return new CleanseSpiritPoisonTrigger(botAI); }
    static Trigger* cleanse_spirit_poison_on_party(PlayerbotAI* botAI)
    {
        return new CleanseSpiritPoisonOnPartyTrigger(botAI);
    }
    static Trigger* cleanse_spirit_disease(PlayerbotAI* botAI) { return new CleanseSpiritDiseaseTrigger(botAI); }
    static Trigger* cleanse_spirit_disease_on_party(PlayerbotAI* botAI)
    {
        return new CleanseSpiritDiseaseOnPartyTrigger(botAI);
    }
    static Trigger* cleanse_spirit_curse(PlayerbotAI* botAI) { return new CleanseSpiritCurseTrigger(botAI); }
    static Trigger* cleanse_spirit_curse_on_party(PlayerbotAI* botAI)
    {
        return new CleanseSpiritCurseOnPartyTrigger(botAI);
    }
    static Trigger* heroism(PlayerbotAI* botAI) { return new HeroismTrigger(botAI); }
    static Trigger* bloodlust(PlayerbotAI* botAI) { return new BloodlustTrigger(botAI); }
    static Trigger* earth_shield_on_main_tank(PlayerbotAI* botAI)
    {
        return new BuffOnMainTankTrigger(botAI, "earth shield");
    }
    static Trigger* water_shield(PlayerbotAI* botAI) { return new WaterShieldTrigger(botAI); }
};

class ShamanAiObjectContextInternal : public NamedObjectContext<Action>
{
public:
    ShamanAiObjectContextInternal()
    {
        creators["water shield"] = &ShamanAiObjectContextInternal::water_shield;
        creators["lightning shield"] = &ShamanAiObjectContextInternal::lightning_shield;
        creators["strength of earth totem"] = &ShamanAiObjectContextInternal::strength_of_earth_totem;
        creators["flametongue totem"] = &ShamanAiObjectContextInternal::flametongue_totem;
        creators["searing totem"] = &ShamanAiObjectContextInternal::searing_totem;
        creators["magma totem"] = &ShamanAiObjectContextInternal::magma_totem;
        creators["windfury totem"] = &ShamanAiObjectContextInternal::windfury_totem;
        creators["grace of air totem"] = &ShamanAiObjectContextInternal::grace_of_air_totem;
        creators["mana spring totem"] = &ShamanAiObjectContextInternal::mana_spring_totem;
        creators["mana tide totem"] = &ShamanAiObjectContextInternal::mana_tide_totem;
        creators["earthbind totem"] = &ShamanAiObjectContextInternal::earthbind_totem;
        creators["healing stream totem"] = &ShamanAiObjectContextInternal::healing_stream_totem;
        creators["wind shear"] = &ShamanAiObjectContextInternal::wind_shear;
        creators["wind shear on enemy healer"] = &ShamanAiObjectContextInternal::wind_shear_on_enemy_healer;
        creators["rockbiter weapon"] = &ShamanAiObjectContextInternal::rockbiter_weapon;
        creators["flametongue weapon"] = &ShamanAiObjectContextInternal::flametongue_weapon;
        creators["frostbrand weapon"] = &ShamanAiObjectContextInternal::frostbrand_weapon;
        creators["windfury weapon"] = &ShamanAiObjectContextInternal::windfury_weapon;
        creators["earthliving weapon"] = &ShamanAiObjectContextInternal::earthliving_weapon;
        creators["purge"] = &ShamanAiObjectContextInternal::purge;
        creators["healing wave"] = &ShamanAiObjectContextInternal::healing_wave;
        creators["lesser healing wave"] = &ShamanAiObjectContextInternal::lesser_healing_wave;
        creators["healing wave on party"] = &ShamanAiObjectContextInternal::healing_wave_on_party;
        creators["lesser healing wave on party"] = &ShamanAiObjectContextInternal::lesser_healing_wave_on_party;
        creators["earth shield"] = &ShamanAiObjectContextInternal::earth_shield;
        creators["earth shield on party"] = &ShamanAiObjectContextInternal::earth_shield_on_party;
        creators["chain heal on party"] = &ShamanAiObjectContextInternal::chain_heal;
        creators["healing surge"] = &ShamanAiObjectContextInternal::healing_surge;
        creators["healing surge on party"] = &ShamanAiObjectContextInternal::healing_surge_on_party;
        creators["healing rain"] = &ShamanAiObjectContextInternal::healing_rain;
        creators["riptide"] = &ShamanAiObjectContextInternal::riptide;
        creators["riptide on party"] = &ShamanAiObjectContextInternal::riptide_on_party;
        creators["stormstrike"] = &ShamanAiObjectContextInternal::stormstrike;
        creators["lava lash"] = &ShamanAiObjectContextInternal::lava_lash;
        creators["fire nova"] = &ShamanAiObjectContextInternal::fire_nova;
        creators["ancestral spirit"] = &ShamanAiObjectContextInternal::ancestral_spirit;
        creators["water walking"] = &ShamanAiObjectContextInternal::water_walking;
        creators["water breathing"] = &ShamanAiObjectContextInternal::water_breathing;
        creators["water walking on party"] = &ShamanAiObjectContextInternal::water_walking_on_party;
        creators["water breathing on party"] = &ShamanAiObjectContextInternal::water_breathing_on_party;
        creators["cleanse spirit"] = &ShamanAiObjectContextInternal::cleanse_spirit;
        creators["cleanse spirit poison on party"] = &ShamanAiObjectContextInternal::cleanse_spirit_poison_on_party;
        creators["cleanse spirit disease on party"] = &ShamanAiObjectContextInternal::cleanse_spirit_disease_on_party;
        creators["cleanse spirit curse on party"] = &ShamanAiObjectContextInternal::cleanse_spirit_curse_on_party;
        creators["flame shock"] = &ShamanAiObjectContextInternal::flame_shock;
        creators["earth shock"] = &ShamanAiObjectContextInternal::earth_shock;
        creators["frost shock"] = &ShamanAiObjectContextInternal::frost_shock;
        creators["chain lightning"] = &ShamanAiObjectContextInternal::chain_lightning;
        creators["lightning bolt"] = &ShamanAiObjectContextInternal::lightning_bolt;
        creators["elemental blast"] = &ShamanAiObjectContextInternal::elemental_blast;
        creators["earthquake"] = &ShamanAiObjectContextInternal::earthquake;
        creators["thunderstorm"] = &ShamanAiObjectContextInternal::thunderstorm;
        creators["heroism"] = &ShamanAiObjectContextInternal::heroism;
        creators["bloodlust"] = &ShamanAiObjectContextInternal::bloodlust;
        creators["elemental mastery"] = &ShamanAiObjectContextInternal::elemental_mastery;
        creators["ascendance"] = &ShamanAiObjectContextInternal::ascendance;
        creators["unleash elements"] = &ShamanAiObjectContextInternal::unleash_elements;
        creators["cure disease"] = &ShamanAiObjectContextInternal::cure_disease;
        creators["cure disease on party"] = &ShamanAiObjectContextInternal::cure_disease_on_party;
        creators["cure poison"] = &ShamanAiObjectContextInternal::cure_poison;
        creators["cure poison on party"] = &ShamanAiObjectContextInternal::cure_poison_on_party;
        creators["lava burst"] = &ShamanAiObjectContextInternal::lava_burst;
        creators["earth shield on main tank"] = &ShamanAiObjectContextInternal::earth_shield_on_main_tank;
        creators["fire elemental totem"] = &ShamanAiObjectContextInternal::fire_elemental_totem;
        creators["fire elemental totem melee"] = &ShamanAiObjectContextInternal::fire_elemental_totem_melee;
        creators["earth elemental totem"] = &ShamanAiObjectContextInternal::earth_elemental_totem;
        creators["healing tide totem"] = &ShamanAiObjectContextInternal::healing_tide_totem;
        creators["spirit link totem"] = &ShamanAiObjectContextInternal::spirit_link_totem;
        creators["totem of wrath"] = &ShamanAiObjectContextInternal::totem_of_wrath;
        creators["wrath of air totem"] = &ShamanAiObjectContextInternal::wrath_of_air_totem;
        creators["shamanistic rage"] = &ShamanAiObjectContextInternal::shamanistic_rage;
        creators["feral spirit"] = &ShamanAiObjectContextInternal::feral_spirit;
    }

private:
    static Action* heroism(PlayerbotAI* botAI) { return new CastHeroismAction(botAI); }
    static Action* bloodlust(PlayerbotAI* botAI) { return new CastBloodlustAction(botAI); }
    static Action* elemental_mastery(PlayerbotAI* botAI) { return new CastElementalMasteryAction(botAI); }
    static Action* ascendance(PlayerbotAI* botAI) { return new CastAscendanceAction(botAI); }
    static Action* unleash_elements(PlayerbotAI* botAI) { return new CastUnleashElementsAction(botAI); }
    static Action* thunderstorm(PlayerbotAI* botAI) { return new CastThunderstormAction(botAI); }
    static Action* lightning_bolt(PlayerbotAI* botAI) { return new CastLightningBoltAction(botAI); }
    static Action* chain_lightning(PlayerbotAI* botAI) { return new CastChainLightningAction(botAI); }
    static Action* elemental_blast(PlayerbotAI* botAI) { return new CastElementalBlastAction(botAI); }
    static Action* earthquake(PlayerbotAI* botAI) { return new CastEarthquakeAction(botAI); }
    static Action* frost_shock(PlayerbotAI* botAI) { return new CastFrostShockAction(botAI); }
    static Action* earth_shock(PlayerbotAI* botAI) { return new CastEarthShockAction(botAI); }
    static Action* flame_shock(PlayerbotAI* botAI) { return new CastFlameShockAction(botAI); }
    static Action* cleanse_spirit_poison_on_party(PlayerbotAI* botAI)
    {
        return new CastCleanseSpiritPoisonOnPartyAction(botAI);
    }
    static Action* cleanse_spirit_disease_on_party(PlayerbotAI* botAI)
    {
        return new CastCleanseSpiritDiseaseOnPartyAction(botAI);
    }
    static Action* cleanse_spirit_curse_on_party(PlayerbotAI* botAI)
    {
        return new CastCleanseSpiritCurseOnPartyAction(botAI);
    }
    static Action* cleanse_spirit(PlayerbotAI* botAI) { return new CastCleanseSpiritAction(botAI); }
    static Action* water_walking(PlayerbotAI* botAI) { return new CastWaterWalkingAction(botAI); }
    static Action* water_breathing(PlayerbotAI* botAI) { return new CastWaterBreathingAction(botAI); }
    static Action* water_walking_on_party(PlayerbotAI* botAI) { return new CastWaterWalkingOnPartyAction(botAI); }
    static Action* water_breathing_on_party(PlayerbotAI* botAI) { return new CastWaterBreathingOnPartyAction(botAI); }
    static Action* water_shield(PlayerbotAI* botAI) { return new CastWaterShieldAction(botAI); }
    static Action* lightning_shield(PlayerbotAI* botAI) { return new CastLightningShieldAction(botAI); }
    static Action* strength_of_earth_totem(PlayerbotAI* botAI) { return new CastStrengthOfEarthTotemAction(botAI); }
    static Action* flametongue_totem(PlayerbotAI* botAI) { return new CastFlametongueTotemAction(botAI); }
    static Action* magma_totem(PlayerbotAI* botAI) { return new CastMagmaTotemAction(botAI); }
    static Action* searing_totem(PlayerbotAI* botAI) { return new CastSearingTotemAction(botAI); }
    static Action* fire_nova(PlayerbotAI* botAI) { return new CastFireNovaAction(botAI); }
    static Action* windfury_totem(PlayerbotAI* botAI) { return new CastWindfuryTotemAction(botAI); }
    static Action* grace_of_air_totem(PlayerbotAI* botAI) { return new CastGraceOfAirTotemAction(botAI); }
    static Action* mana_spring_totem(PlayerbotAI* botAI) { return new CastManaSpringTotemAction(botAI); }
    static Action* mana_tide_totem(PlayerbotAI* botAI) { return new CastManaTideTotemAction(botAI); }
    static Action* earthbind_totem(PlayerbotAI* botAI) { return new CastEarthbindTotemAction(botAI); }
    static Action* healing_stream_totem(PlayerbotAI* botAI) { return new CastHealingStreamTotemAction(botAI); }
    static Action* wind_shear(PlayerbotAI* botAI) { return new CastWindShearAction(botAI); }
    static Action* rockbiter_weapon(PlayerbotAI* botAI) { return new CastRockbiterWeaponAction(botAI); }
    static Action* flametongue_weapon(PlayerbotAI* botAI) { return new CastFlametongueWeaponAction(botAI); }
    static Action* frostbrand_weapon(PlayerbotAI* botAI) { return new CastFrostbrandWeaponAction(botAI); }
    static Action* windfury_weapon(PlayerbotAI* botAI) { return new CastWindfuryWeaponAction(botAI); }
    static Action* earthliving_weapon(PlayerbotAI* botAI) { return new CastEarthlivingWeaponAction(botAI); }
    static Action* purge(PlayerbotAI* botAI) { return new CastPurgeAction(botAI); }
    static Action* healing_wave(PlayerbotAI* botAI) { return new CastHealingWaveAction(botAI); }
    static Action* lesser_healing_wave(PlayerbotAI* botAI) { return new CastLesserHealingWaveAction(botAI); }
    static Action* healing_wave_on_party(PlayerbotAI* botAI) { return new CastHealingWaveOnPartyAction(botAI); }
    static Action* lesser_healing_wave_on_party(PlayerbotAI* botAI)
    {
        return new CastLesserHealingWaveOnPartyAction(botAI);
    }
    static Action* earth_shield(PlayerbotAI* botAI) { return new CastEarthShieldAction(botAI); }
    static Action* earth_shield_on_party(PlayerbotAI* botAI) { return new CastEarthShieldOnPartyAction(botAI); }
    static Action* chain_heal(PlayerbotAI* botAI) { return new CastChainHealAction(botAI); }
    static Action* healing_surge(PlayerbotAI* botAI) { return new CastHealingSurgeAction(botAI); }
    static Action* healing_surge_on_party(PlayerbotAI* botAI) { return new CastHealingSurgeOnPartyAction(botAI); }
    static Action* healing_rain(PlayerbotAI* botAI) { return new CastHealingRainAction(botAI); }
    static Action* riptide(PlayerbotAI* botAI) { return new CastRiptideAction(botAI); }
    static Action* riptide_on_party(PlayerbotAI* botAI) { return new CastRiptideOnPartyAction(botAI); }
    static Action* stormstrike(PlayerbotAI* botAI) { return new CastStormstrikeAction(botAI); }
    static Action* lava_lash(PlayerbotAI* botAI) { return new CastLavaLashAction(botAI); }
    static Action* ancestral_spirit(PlayerbotAI* botAI) { return new CastAncestralSpiritAction(botAI); }
    static Action* wind_shear_on_enemy_healer(PlayerbotAI* botAI)
    {
        return new CastWindShearOnEnemyHealerAction(botAI);
    }
    static Action* cure_poison(PlayerbotAI* botAI) { return new CastCurePoisonActionSham(botAI); }
    static Action* cure_poison_on_party(PlayerbotAI* botAI) { return new CastCurePoisonOnPartyActionSham(botAI); }
    static Action* cure_disease(PlayerbotAI* botAI) { return new CastCureDiseaseActionSham(botAI); }
    static Action* cure_disease_on_party(PlayerbotAI* botAI) { return new CastCureDiseaseOnPartyActionSham(botAI); }
    static Action* lava_burst(PlayerbotAI* ai) { return new CastLavaBurstAction(ai); }
    static Action* earth_shield_on_main_tank(PlayerbotAI* ai) { return new CastEarthShieldOnMainTankAction(ai); }
    static Action* totem_of_wrath(PlayerbotAI* ai) { return new CastTotemOfWrathAction(ai); }
    static Action* fire_elemental_totem(PlayerbotAI* ai) { return new CastFireElementalTotemAction(ai); }
    static Action* fire_elemental_totem_melee(PlayerbotAI* ai) { return new CastFireElementalTotemMeleeAction(ai); }
    static Action* earth_elemental_totem(PlayerbotAI* ai) { return new CastEarthElementalTotemAction(ai); }
    static Action* healing_tide_totem(PlayerbotAI* ai) { return new CastHealingTideTotemAction(ai); }
    static Action* spirit_link_totem(PlayerbotAI* ai) { return new CastSpiritLinkTotemAction(ai); }
    static Action* wrath_of_air_totem(PlayerbotAI* ai) { return new CastWrathOfAirTotemAction(ai); }
    static Action* shamanistic_rage(PlayerbotAI* ai) { return new CastShamanisticRageAction(ai); }
    static Action* feral_spirit(PlayerbotAI* ai) { return new CastFeralSpiritAction(ai); }
};

ShamanAiObjectContext::ShamanAiObjectContext(PlayerbotAI* botAI) : AiObjectContext(botAI)
{
    strategyContexts.Add(new ShamanStrategyFactoryInternal());
    strategyContexts.Add(new ShamanCombatStrategyFactoryInternal());
    strategyContexts.Add(new ShamanBuffStrategyFactoryInternal());
    actionContexts.Add(new ShamanAiObjectContextInternal());
    triggerContexts.Add(new ShamanATriggerFactoryInternal());
}
