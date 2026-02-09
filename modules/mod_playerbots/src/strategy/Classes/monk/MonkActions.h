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

#ifndef _PLAYERBOT_MONKACTIONS_H
#define _PLAYERBOT_MONKACTIONS_H

#include "GenericSpellActions.h"
#include "Playerbots.h"

class PlayerbotAI;

// Core damage/utility
MELEE_ACTION(CastJabAction, "jab");
MELEE_ACTION(CastTigerPalmAction, "tiger palm");
MELEE_ACTION(CastBlackoutKickAction, "blackout kick");
MELEE_ACTION(CastRisingSunKickAction, "rising sun kick");
MELEE_ACTION(CastFistsOfFuryAction, "fists of fury");
MELEE_ACTION(CastSpinningCraneKickAction, "spinning crane kick");
MELEE_ACTION(CastKegSmashAction, "keg smash");
MELEE_ACTION(CastBreathOfFireAction, "breath of fire");
MELEE_ACTION(CastRushingJadeWindAction, "rushing jade wind");
MELEE_ACTION(CastTouchOfDeathAction, "touch of death");

SPELL_ACTION(CastChiWaveAction, "chi wave");
SPELL_ACTION(CastChiBurstAction, "chi burst");
SPELL_ACTION(CastZenSphereAction, "zen sphere");
SPELL_ACTION(CastSpearHandStrikeAction, "spear hand strike");
SPELL_ACTION(CastExpelHarmAction, "expel harm");
SPELL_ACTION(CastDizzyingHazeAction, "dizzying haze");
SPELL_ACTION(CastClashAction, "clash");
SPELL_ACTION(CastHealingSphereAction, "healing sphere");
SPELL_ACTION(CastChiTorpedoAction, "chi torpedo");

// Defensive/utility buffs
BUFF_ACTION(CastGuardAction, "guard");
BUFF_ACTION(CastPurifyingBrewAction, "purifying brew");
BUFF_ACTION(CastElusiveBrewAction, "elusive brew");
BUFF_ACTION(CastFortifyingBrewAction, "fortifying brew");
BUFF_ACTION(CastEnergizingBrewAction, "energizing brew");
BUFF_ACTION(CastTigerEyeBrewAction, "tiger eye brew");

BUFF_ACTION(CastLegacyOfTheEmperorAction, "legacy of the emperor");
BUFF_ACTION(CastStanceOfTheFierceTigerAction, "stance of the fierce tiger");
BUFF_ACTION(CastStanceOfTheSturdyOxAction, "stance of the sturdy ox");
BUFF_ACTION(CastStanceOfTheWiseSerpentAction, "stance of the wise serpent");
BUFF_ACTION(CastSummonJadeSerpentStatueAction, "summon jade serpent statue");
BUFF_ACTION(CastSummonBlackOxStatueAction, "summon black ox statue");
BUFF_ACTION(CastManaTeaAction, "mana tea");
BUFF_ACTION(CastThunderFocusTeaAction, "thunder focus tea");
BUFF_ACTION(CastChiBrewAction, "chi brew");
BUFF_ACTION(CastInvokeXuenAction, "invoke xuen");

// Healing
HEAL_PARTY_ACTION(CastSoothingMistOnPartyAction, "soothing mist", 12.0f, HealingManaEfficiency::VERY_HIGH);
HEAL_PARTY_ACTION(CastSurgingMistOnPartyAction, "surging mist", 25.0f, HealingManaEfficiency::LOW);
HEAL_PARTY_ACTION(CastEnvelopingMistOnPartyAction, "enveloping mist", 20.0f, HealingManaEfficiency::MEDIUM);
HEAL_PARTY_ACTION(CastRenewingMistOnPartyAction, "renewing mist", 12.0f, HealingManaEfficiency::VERY_HIGH);
AOE_HEAL_ACTION(CastUpliftAction, "uplift", 20.0f, HealingManaEfficiency::MEDIUM);
AOE_HEAL_ACTION(CastRevivalAction, "revival", 30.0f, HealingManaEfficiency::HIGH);
BUFF_PARTY_ACTION(CastLifeCocoonOnPartyAction, "life cocoon");
RESS_ACTION(CastResuscitateAction, "resuscitate");

// Dispel
CURE_ACTION(CastDetoxAction, "detox");
CURE_PARTY_ACTION(CastDetoxOnPartyAction, "detox", DISPEL_DISEASE | DISPEL_POISON | DISPEL_MAGIC);

#endif
