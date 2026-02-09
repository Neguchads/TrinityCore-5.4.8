/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "MonkAiObjectContext.h"

#include "BrewmasterMonkStrategy.h"
#include "GenericMonkNonCombatStrategy.h"
#include "GenericMonkStrategy.h"
#include "MistweaverMonkStrategy.h"
#include "MonkActions.h"
#include "MonkTriggers.h"
#include "NamedObjectContext.h"
#include "Playerbots.h"
#include "WindwalkerMonkStrategy.h"

class MonkStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    MonkStrategyFactoryInternal()
    {
        creators["nc"] = &MonkStrategyFactoryInternal::nc;
        creators["cure"] = &MonkStrategyFactoryInternal::cure;
        creators["aoe"] = &MonkStrategyFactoryInternal::aoe;
        creators["melee aoe"] = &MonkStrategyFactoryInternal::aoe;
    }

private:
    static Strategy* nc(PlayerbotAI* botAI) { return new GenericMonkNonCombatStrategy(botAI); }
    static Strategy* cure(PlayerbotAI* botAI) { return new MonkCureStrategy(botAI); }
    static Strategy* aoe(PlayerbotAI* botAI) { return new MonkAoeStrategy(botAI); }
};

class MonkCombatStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    MonkCombatStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["melee"] = &MonkCombatStrategyFactoryInternal::dps;
        creators["dps"] = &MonkCombatStrategyFactoryInternal::dps;
        creators["tank"] = &MonkCombatStrategyFactoryInternal::tank;
        creators["heal"] = &MonkCombatStrategyFactoryInternal::heal;
    }

private:
    static Strategy* dps(PlayerbotAI* botAI) { return new WindwalkerMonkStrategy(botAI); }
    static Strategy* tank(PlayerbotAI* botAI) { return new BrewmasterMonkStrategy(botAI); }
    static Strategy* heal(PlayerbotAI* botAI) { return new MistweaverMonkStrategy(botAI); }
};

class MonkTriggerFactoryInternal : public NamedObjectContext<Trigger>
{
public:
    MonkTriggerFactoryInternal()
    {
        creators["tiger power"] = &MonkTriggerFactoryInternal::tiger_power;
        creators["rising sun kick"] = &MonkTriggerFactoryInternal::rising_sun_kick;
        creators["shuffle"] = &MonkTriggerFactoryInternal::shuffle;
        creators["light stagger"] = &MonkTriggerFactoryInternal::light_stagger;
        creators["moderate stagger"] = &MonkTriggerFactoryInternal::moderate_stagger;
        creators["heavy stagger"] = &MonkTriggerFactoryInternal::heavy_stagger;
        creators["spear hand strike"] = &MonkTriggerFactoryInternal::spear_hand_strike;
        creators["legacy of the emperor"] = &MonkTriggerFactoryInternal::legacy_of_the_emperor;
        creators["stance of the fierce tiger"] = &MonkTriggerFactoryInternal::stance_fierce_tiger;
        creators["stance of the sturdy ox"] = &MonkTriggerFactoryInternal::stance_sturdy_ox;
        creators["stance of the wise serpent"] = &MonkTriggerFactoryInternal::stance_wise_serpent;
        creators["detox"] = &MonkTriggerFactoryInternal::detox;
        creators["party member detox"] = &MonkTriggerFactoryInternal::party_member_detox;
        creators["renewing mist on party"] = &MonkTriggerFactoryInternal::renewing_mist_on_party;
    }

private:
    static Trigger* tiger_power(PlayerbotAI* botAI) { return new TigerPowerTrigger(botAI); }
    static Trigger* rising_sun_kick(PlayerbotAI* botAI) { return new RisingSunKickDebuffTrigger(botAI); }
    static Trigger* shuffle(PlayerbotAI* botAI) { return new ShuffleTrigger(botAI); }
    static Trigger* light_stagger(PlayerbotAI* botAI) { return new LightStaggerTrigger(botAI); }
    static Trigger* moderate_stagger(PlayerbotAI* botAI) { return new ModerateStaggerTrigger(botAI); }
    static Trigger* heavy_stagger(PlayerbotAI* botAI) { return new HeavyStaggerTrigger(botAI); }
    static Trigger* spear_hand_strike(PlayerbotAI* botAI) { return new SpearHandStrikeInterruptTrigger(botAI); }
    static Trigger* legacy_of_the_emperor(PlayerbotAI* botAI) { return new LegacyOfTheEmperorTrigger(botAI); }
    static Trigger* stance_fierce_tiger(PlayerbotAI* botAI) { return new StanceFierceTigerTrigger(botAI); }
    static Trigger* stance_sturdy_ox(PlayerbotAI* botAI) { return new StanceSturdyOxTrigger(botAI); }
    static Trigger* stance_wise_serpent(PlayerbotAI* botAI) { return new StanceWiseSerpentTrigger(botAI); }
    static Trigger* detox(PlayerbotAI* botAI) { return new DetoxTrigger(botAI); }
    static Trigger* party_member_detox(PlayerbotAI* botAI) { return new DetoxPartyTrigger(botAI); }
    static Trigger* renewing_mist_on_party(PlayerbotAI* botAI) { return new RenewingMistOnPartyTrigger(botAI); }
};

class MonkAiObjectContextInternal : public NamedObjectContext<Action>
{
public:
    MonkAiObjectContextInternal()
    {
        creators["jab"] = &MonkAiObjectContextInternal::jab;
        creators["tiger palm"] = &MonkAiObjectContextInternal::tiger_palm;
        creators["blackout kick"] = &MonkAiObjectContextInternal::blackout_kick;
        creators["rising sun kick"] = &MonkAiObjectContextInternal::rising_sun_kick;
        creators["fists of fury"] = &MonkAiObjectContextInternal::fists_of_fury;
        creators["spinning crane kick"] = &MonkAiObjectContextInternal::spinning_crane_kick;
        creators["keg smash"] = &MonkAiObjectContextInternal::keg_smash;
        creators["breath of fire"] = &MonkAiObjectContextInternal::breath_of_fire;
        creators["rushing jade wind"] = &MonkAiObjectContextInternal::rushing_jade_wind;
        creators["touch of death"] = &MonkAiObjectContextInternal::touch_of_death;
        creators["chi wave"] = &MonkAiObjectContextInternal::chi_wave;
        creators["chi burst"] = &MonkAiObjectContextInternal::chi_burst;
        creators["zen sphere"] = &MonkAiObjectContextInternal::zen_sphere;
        creators["spear hand strike"] = &MonkAiObjectContextInternal::spear_hand_strike;
        creators["expel harm"] = &MonkAiObjectContextInternal::expel_harm;
        creators["dizzying haze"] = &MonkAiObjectContextInternal::dizzying_haze;
        creators["clash"] = &MonkAiObjectContextInternal::clash;
        creators["healing sphere"] = &MonkAiObjectContextInternal::healing_sphere;
        creators["chi torpedo"] = &MonkAiObjectContextInternal::chi_torpedo;
        creators["guard"] = &MonkAiObjectContextInternal::guard;
        creators["purifying brew"] = &MonkAiObjectContextInternal::purifying_brew;
        creators["elusive brew"] = &MonkAiObjectContextInternal::elusive_brew;
        creators["fortifying brew"] = &MonkAiObjectContextInternal::fortifying_brew;
        creators["energizing brew"] = &MonkAiObjectContextInternal::energizing_brew;
        creators["tiger eye brew"] = &MonkAiObjectContextInternal::tiger_eye_brew;
        creators["legacy of the emperor"] = &MonkAiObjectContextInternal::legacy_of_the_emperor;
        creators["stance of the fierce tiger"] = &MonkAiObjectContextInternal::stance_of_the_fierce_tiger;
        creators["stance of the sturdy ox"] = &MonkAiObjectContextInternal::stance_of_the_sturdy_ox;
        creators["stance of the wise serpent"] = &MonkAiObjectContextInternal::stance_of_the_wise_serpent;
        creators["summon jade serpent statue"] = &MonkAiObjectContextInternal::summon_jade_serpent_statue;
        creators["summon black ox statue"] = &MonkAiObjectContextInternal::summon_black_ox_statue;
        creators["mana tea"] = &MonkAiObjectContextInternal::mana_tea;
        creators["thunder focus tea"] = &MonkAiObjectContextInternal::thunder_focus_tea;
        creators["chi brew"] = &MonkAiObjectContextInternal::chi_brew;
        creators["invoke xuen"] = &MonkAiObjectContextInternal::invoke_xuen;

        creators["soothing mist on party"] = &MonkAiObjectContextInternal::soothing_mist_on_party;
        creators["surging mist on party"] = &MonkAiObjectContextInternal::surging_mist_on_party;
        creators["enveloping mist on party"] = &MonkAiObjectContextInternal::enveloping_mist_on_party;
        creators["renewing mist on party"] = &MonkAiObjectContextInternal::renewing_mist_on_party;
        creators["uplift"] = &MonkAiObjectContextInternal::uplift;
        creators["revival"] = &MonkAiObjectContextInternal::revival;
        creators["life cocoon on party"] = &MonkAiObjectContextInternal::life_cocoon_on_party;
        creators["resuscitate"] = &MonkAiObjectContextInternal::resuscitate;

        creators["detox"] = &MonkAiObjectContextInternal::detox;
        creators["detox on party"] = &MonkAiObjectContextInternal::detox_on_party;
    }

private:
    static Action* jab(PlayerbotAI* botAI) { return new CastJabAction(botAI); }
    static Action* tiger_palm(PlayerbotAI* botAI) { return new CastTigerPalmAction(botAI); }
    static Action* blackout_kick(PlayerbotAI* botAI) { return new CastBlackoutKickAction(botAI); }
    static Action* rising_sun_kick(PlayerbotAI* botAI) { return new CastRisingSunKickAction(botAI); }
    static Action* fists_of_fury(PlayerbotAI* botAI) { return new CastFistsOfFuryAction(botAI); }
    static Action* spinning_crane_kick(PlayerbotAI* botAI) { return new CastSpinningCraneKickAction(botAI); }
    static Action* keg_smash(PlayerbotAI* botAI) { return new CastKegSmashAction(botAI); }
    static Action* breath_of_fire(PlayerbotAI* botAI) { return new CastBreathOfFireAction(botAI); }
    static Action* rushing_jade_wind(PlayerbotAI* botAI) { return new CastRushingJadeWindAction(botAI); }
    static Action* touch_of_death(PlayerbotAI* botAI) { return new CastTouchOfDeathAction(botAI); }
    static Action* chi_wave(PlayerbotAI* botAI) { return new CastChiWaveAction(botAI); }
    static Action* chi_burst(PlayerbotAI* botAI) { return new CastChiBurstAction(botAI); }
    static Action* zen_sphere(PlayerbotAI* botAI) { return new CastZenSphereAction(botAI); }
    static Action* spear_hand_strike(PlayerbotAI* botAI) { return new CastSpearHandStrikeAction(botAI); }
    static Action* expel_harm(PlayerbotAI* botAI) { return new CastExpelHarmAction(botAI); }
    static Action* dizzying_haze(PlayerbotAI* botAI) { return new CastDizzyingHazeAction(botAI); }
    static Action* clash(PlayerbotAI* botAI) { return new CastClashAction(botAI); }
    static Action* healing_sphere(PlayerbotAI* botAI) { return new CastHealingSphereAction(botAI); }
    static Action* chi_torpedo(PlayerbotAI* botAI) { return new CastChiTorpedoAction(botAI); }
    static Action* guard(PlayerbotAI* botAI) { return new CastGuardAction(botAI); }
    static Action* purifying_brew(PlayerbotAI* botAI) { return new CastPurifyingBrewAction(botAI); }
    static Action* elusive_brew(PlayerbotAI* botAI) { return new CastElusiveBrewAction(botAI); }
    static Action* fortifying_brew(PlayerbotAI* botAI) { return new CastFortifyingBrewAction(botAI); }
    static Action* energizing_brew(PlayerbotAI* botAI) { return new CastEnergizingBrewAction(botAI); }
    static Action* tiger_eye_brew(PlayerbotAI* botAI) { return new CastTigerEyeBrewAction(botAI); }
    static Action* legacy_of_the_emperor(PlayerbotAI* botAI) { return new CastLegacyOfTheEmperorAction(botAI); }
    static Action* stance_of_the_fierce_tiger(PlayerbotAI* botAI) { return new CastStanceOfTheFierceTigerAction(botAI); }
    static Action* stance_of_the_sturdy_ox(PlayerbotAI* botAI) { return new CastStanceOfTheSturdyOxAction(botAI); }
    static Action* stance_of_the_wise_serpent(PlayerbotAI* botAI) { return new CastStanceOfTheWiseSerpentAction(botAI); }
    static Action* summon_jade_serpent_statue(PlayerbotAI* botAI) { return new CastSummonJadeSerpentStatueAction(botAI); }
    static Action* summon_black_ox_statue(PlayerbotAI* botAI) { return new CastSummonBlackOxStatueAction(botAI); }
    static Action* mana_tea(PlayerbotAI* botAI) { return new CastManaTeaAction(botAI); }
    static Action* thunder_focus_tea(PlayerbotAI* botAI) { return new CastThunderFocusTeaAction(botAI); }
    static Action* chi_brew(PlayerbotAI* botAI) { return new CastChiBrewAction(botAI); }
    static Action* invoke_xuen(PlayerbotAI* botAI) { return new CastInvokeXuenAction(botAI); }

    static Action* soothing_mist_on_party(PlayerbotAI* botAI) { return new CastSoothingMistOnPartyAction(botAI); }
    static Action* surging_mist_on_party(PlayerbotAI* botAI) { return new CastSurgingMistOnPartyAction(botAI); }
    static Action* enveloping_mist_on_party(PlayerbotAI* botAI) { return new CastEnvelopingMistOnPartyAction(botAI); }
    static Action* renewing_mist_on_party(PlayerbotAI* botAI) { return new CastRenewingMistOnPartyAction(botAI); }
    static Action* uplift(PlayerbotAI* botAI) { return new CastUpliftAction(botAI); }
    static Action* revival(PlayerbotAI* botAI) { return new CastRevivalAction(botAI); }
    static Action* life_cocoon_on_party(PlayerbotAI* botAI) { return new CastLifeCocoonOnPartyAction(botAI); }
    static Action* resuscitate(PlayerbotAI* botAI) { return new CastResuscitateAction(botAI); }

    static Action* detox(PlayerbotAI* botAI) { return new CastDetoxAction(botAI); }
    static Action* detox_on_party(PlayerbotAI* botAI) { return new CastDetoxOnPartyAction(botAI); }
};

MonkAiObjectContext::MonkAiObjectContext(PlayerbotAI* botAI) : AiObjectContext(botAI)
{
    strategyContexts.Add(new MonkStrategyFactoryInternal());
    strategyContexts.Add(new MonkCombatStrategyFactoryInternal());
    actionContexts.Add(new MonkAiObjectContextInternal());
    triggerContexts.Add(new MonkTriggerFactoryInternal());
}
