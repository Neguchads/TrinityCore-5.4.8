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

#ifndef _PLAYERBOT_MONKTRIGGERS_H
#define _PLAYERBOT_MONKTRIGGERS_H

#include "CureTriggers.h"
#include "GenericTriggers.h"
#include "Playerbots.h"

class PlayerbotAI;

BUFF_TRIGGER(TigerPowerTrigger, "tiger power");
DEBUFF_TRIGGER(RisingSunKickDebuffTrigger, "rising sun kick");
BUFF_TRIGGER(ShuffleTrigger, "shuffle");

DEBUFF_TRIGGER(LightStaggerTrigger, "light stagger");
DEBUFF_TRIGGER(ModerateStaggerTrigger, "moderate stagger");
DEBUFF_TRIGGER(HeavyStaggerTrigger, "heavy stagger");

INTERRUPT_TRIGGER(SpearHandStrikeInterruptTrigger, "spear hand strike");

BUFF_TRIGGER(StanceFierceTigerTrigger, "stance of the fierce tiger");
BUFF_TRIGGER(StanceSturdyOxTrigger, "stance of the sturdy ox");
BUFF_TRIGGER(StanceWiseSerpentTrigger, "stance of the wise serpent");
BUFF_TRIGGER(LegacyOfTheEmperorTrigger, "legacy of the emperor");

CURE_TRIGGER(DetoxTrigger, "detox", DISPEL_DISEASE | DISPEL_POISON | DISPEL_MAGIC);
CURE_PARTY_TRIGGER(DetoxPartyTrigger, "detox", DISPEL_DISEASE | DISPEL_POISON | DISPEL_MAGIC);

BUFF_PARTY_TRIGGER(RenewingMistOnPartyTrigger, "renewing mist");

class TigerEyeBrewTrigger : public BoostTrigger
{
public:
    TigerEyeBrewTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "tiger eye brew") {}
};

class InvokeXuenTrigger : public BoostTrigger
{
public:
    InvokeXuenTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "invoke xuen") {}
};

#endif
