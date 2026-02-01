/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
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

#endif
