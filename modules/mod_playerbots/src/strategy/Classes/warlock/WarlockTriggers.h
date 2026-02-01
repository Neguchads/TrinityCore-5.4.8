

#ifndef _PLAYERBOT_WARLOCKTRIGGERS_H
#define _PLAYERBOT_WARLOCKTRIGGERS_H

#include "GenericTriggers.h"
#include "PlayerbotAI.h"

class PlayerbotAI;

// -- TALENTS

BUFF_TRIGGER_TIMED(SoulLinkTrigger, "soul link", 5000);

// -- GENERIC

BUFF_TRIGGER_TIMED(DarkIntentTrigger, "dark intent", 5000);
class SoulstoneTrigger : public Trigger
{
public:
    SoulstoneTrigger(PlayerbotAI* botAI) : Trigger(botAI, "soulstone") {}
    bool IsActive() override;
};
class CurseOfAgonyTrigger : public DebuffTrigger
{
public:
    CurseOfAgonyTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "agony", 1, true, 20.0f) {}
};

class CurseOfElementsTrigger : public DebuffTrigger
{
public:
    CurseOfElementsTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "curse of the elements", 1, false) {}
    bool IsActive() override
    {
        return DebuffTrigger::IsActive() && !botAI->HasAura("curse of the elements", GetTarget()) &&
            !botAI->HasAura("curse of enfeeblement", GetTarget(), false, true);
    }
};

class CurseOfEnfeeblementTrigger : public DebuffTrigger
{
public:
    CurseOfEnfeeblementTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "curse of enfeeblement", 1, false) {}
    bool IsActive() override
    {
        return DebuffTrigger::IsActive() && !botAI->HasAura("curse of enfeeblement", GetTarget()) &&
            !botAI->HasAura("curse of the elements", GetTarget(), false, true);
    }
};

class CorruptionTrigger : public DebuffTrigger
{
public:
    CorruptionTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "corruption", 1, true) {}
    bool IsActive() override;
};
DEBUFF_CHECKISOWNER_TRIGGER(UnstableAfflictionTrigger, "unstable affliction");

class LifeTapGlyphBuffTrigger : public Trigger
{
public:
    LifeTapGlyphBuffTrigger(PlayerbotAI* botAI) : Trigger(botAI, "life tap glyph buff") {}
    bool IsActive() override;
};

// -- DEMONO
class MetamorphosisTrigger : public BuffTrigger
{
public:
    MetamorphosisTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "metamorphosis", true) {}
    bool IsActive() override;
};

class MoltenCoreTrigger : public HasAuraTrigger
{
public:
    MoltenCoreTrigger(PlayerbotAI* ai) : HasAuraTrigger(ai, "molten core") {}
};
CAN_CAST_TRIGGER(HandOfGuldanTrigger, "hand of guldan");
CAN_CAST_TRIGGER(SoulFireTrigger, "soul fire");
CAN_CAST_TRIGGER(HauntTrigger, "haunt");

// -- Destru
DEBUFF_CHECKISOWNER_TRIGGER(ImmolateTrigger, "immolate");
class BackdraftTrigger : public HasAuraStackTrigger
{
public:
    BackdraftTrigger(PlayerbotAI* ai) : HasAuraStackTrigger(ai, "backdraft", 1) {}
};
CAN_CAST_TRIGGER(ConflagrateTrigger, "conflagrate");
class FireAndBrimeStoneTrigger : public BuffTrigger
{
public:
    FireAndBrimeStoneTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "fire and brimstone", true) {}
    bool IsActive() override;
};

// -- Pet
BEGIN_TRIGGER(WarlockPetLowHealthTrigger, Trigger)
END_TRIGGER()
BEGIN_TRIGGER(WarlockPetMediumHealthTrigger, Trigger)
END_TRIGGER()

#endif
