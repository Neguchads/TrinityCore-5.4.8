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

class AdrenalineRushTrigger : public BoostTrigger
{
public:
    AdrenalineRushTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "adrenaline rush") {}
};

class ShadowBladesTrigger : public BoostTrigger
{
public:
    ShadowBladesTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "shadow blades") {}
};

class ShadowDanceTrigger : public BoostTrigger
{
public:
    ShadowDanceTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "shadow dance") {}
};

class KillingSpreeTrigger : public BoostTrigger
{
public:
    KillingSpreeTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "killing spree") {}
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

class KidneyShotPvpTrigger : public Trigger
{
public:
    KidneyShotPvpTrigger(PlayerbotAI* botAI) : Trigger(botAI, "kidney shot pvp") {}

    bool IsActive() override;
};

class StealthPvpOpenerTrigger : public Trigger
{
public:
    StealthPvpOpenerTrigger(PlayerbotAI* botAI) : Trigger(botAI, "stealth pvp opener") {}

    bool IsActive() override;
};

#endif
