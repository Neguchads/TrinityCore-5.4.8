#ifndef _PLAYERBOT_PRIESTTRIGGERS_H
#define _PLAYERBOT_PRIESTTRIGGERS_H

#include "GenericTriggers.h"
#include "SharedDefines.h"

class PlayerbotAI;

class ShadowformTrigger : public BuffTrigger
{
public:
    ShadowformTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "shadowform") {}
};

class InnerFireTrigger : public BuffTrigger
{
public:
    InnerFireTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "inner fire") {}
};

class InnerWillTrigger : public BuffTrigger
{
public:
    InnerWillTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "inner will") {}
};

class HolyFireTrigger : public DebuffTrigger
{
public:
    HolyFireTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "holy fire", 1, true) {}
};

class PowerWordSolaceTrigger : public DebuffTrigger
{
public:
    PowerWordSolaceTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "power word: solace", 1, true) {}
};

class VampiricTouchTrigger : public DebuffTrigger
{
public:
    VampiricTouchTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "vampiric touch", 1, true) {}
};

class ShadowWordPainTrigger : public DebuffTrigger
{
public:
    ShadowWordPainTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "shadow word: pain", 1, true) {}
};

class EvangelismTrigger : public HasAuraStackTrigger
{
public:
    EvangelismTrigger(PlayerbotAI* botAI, uint8 minStacks) : HasAuraStackTrigger(botAI, "evangelism", minStacks, 1) {}
};

class ShadowOrbsTrigger : public Trigger
{
public:
    ShadowOrbsTrigger(PlayerbotAI* botAI, uint8 minOrbs) : Trigger(botAI, "shadow orbs"), minOrbs(minOrbs) {}
    bool IsActive() override;

private:
    uint8 minOrbs;
};

class SurgeOfDarknessTrigger : public HasAuraTrigger
{
public:
    SurgeOfDarknessTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "surge of darkness") {}
};

#endif
