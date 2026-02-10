

#ifndef _PLAYERBOT_HUNTERTRIGGERS_H
#define _PLAYERBOT_HUNTERTRIGGERS_H

#include "GenericTriggers.h"
#include "Trigger.h"

class PlayerbotAI;

class HunterSerpentStingTrigger : public DebuffTrigger
{
public:
    HunterSerpentStingTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "serpent sting", 1, true) {}
};

class HunterBlackArrowTrigger : public DebuffTrigger
{
public:
    HunterBlackArrowTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "black arrow", 1, true) {}
};

class HuntersMarkTrigger : public DebuffTrigger
{
public:
    HuntersMarkTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "hunter's mark", 1, true) {}
};

class HunterAspectOfTheMonkeyTrigger : public BuffTrigger
{
public:
    HunterAspectOfTheMonkeyTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "aspect of the monkey") {}
};

class HunterAspectOfTheHawkTrigger : public BuffTrigger
{
public:
    HunterAspectOfTheHawkTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "aspect of the hawk") {}
    bool IsActive() override;
};

class HunterAspectOfTheWildTrigger : public BuffTrigger
{
public:
    HunterAspectOfTheWildTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "aspect of the wild") {}
};

class HunterAspectOfTheViperTrigger : public BuffTrigger
{
public:
    HunterAspectOfTheViperTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "aspect of the viper") {}
    bool IsActive() override;
};

class HunterAspectOfThePackTrigger : public BuffTrigger
{
public:
    HunterAspectOfThePackTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "aspect of the pack") {}
    bool IsActive() override;
};

class TrueshotAuraTrigger : public BuffTrigger
{
public:
    TrueshotAuraTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "trueshot aura") {}
};

class NoTrackTrigger : public BuffTrigger
{
public:
    NoTrackTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "no track") {}
    bool IsActive() override;
};

class HunterLockAndLoadTrigger : public HasAuraTrigger
{
public:
    HunterLockAndLoadTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "lock and load") {}
};

class HunterFireTrigger : public HasAuraTrigger
{
public:
    HunterFireTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "fire!") {}
};

class HunterFocusFireTrigger : public Trigger
{
public:
    HunterFocusFireTrigger(PlayerbotAI* botAI) : Trigger(botAI, "focus fire", 1) {}
    bool IsActive() override;
};

class RapidFireTrigger : public BoostTrigger
{
public:
    RapidFireTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "rapid fire") {}
};

class BestialWrathTrigger : public BoostTrigger
{
public:
    BestialWrathTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "bestial wrath") {}
};

class StampedeTrigger : public BoostTrigger
{
public:
    StampedeTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "stampede") {}
};

#endif
