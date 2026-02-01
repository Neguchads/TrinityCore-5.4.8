#ifndef _PLAYERBOT_ROGUETRIGGERS_H
#define _PLAYERBOT_ROGUETRIGGERS_H

#include "GenericTriggers.h"

class PlayerbotAI;

class SliceAndDiceTrigger : public BuffTrigger
{
public:
    SliceAndDiceTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "slice and dice") {}
};

class BladeFlurryTrigger : public BuffTrigger
{
public:
    BladeFlurryTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "blade flurry") {}
};

class AdrenalineRushTrigger : public BuffTrigger
{
public:
    AdrenalineRushTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "adrenaline rush") {}
};

class ShadowBladesTrigger : public BuffTrigger
{
public:
    ShadowBladesTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "shadow blades") {}
};

class ShadowDanceTrigger : public BuffTrigger
{
public:
    ShadowDanceTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "shadow dance") {}
};

class VendettaTrigger : public DebuffTrigger
{
public:
    VendettaTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "vendetta", 1, true) {}
};

class RevealingStrikeTrigger : public DebuffTrigger
{
public:
    RevealingStrikeTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "revealing strike", 1, true) {}
};

class HemorrhageTrigger : public DebuffTrigger
{
public:
    HemorrhageTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "hemorrhage", 1, true) {}
};

class RuptureTrigger : public DebuffTrigger
{
public:
    RuptureTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "rupture", 1, true) {}
};

class BlindsideTrigger : public HasAuraTrigger
{
public:
    BlindsideTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "blindside") {}
};

#endif
