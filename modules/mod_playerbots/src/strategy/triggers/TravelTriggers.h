#ifndef _PLAYERBOT_TRAVELTRIGGERS_H
#define _PLAYERBOT_TRAVELTRIGGERS_H

#include "Trigger.h"

class PlayerbotAI;

class NoTravelTargetTrigger : public Trigger
{
public:
    NoTravelTargetTrigger(PlayerbotAI* botAI) : Trigger(botAI, "no travel target") {}

    bool IsActive() override;
};

class FarFromTravelTargetTrigger : public Trigger
{
public:
    FarFromTravelTargetTrigger(PlayerbotAI* botAI) : Trigger(botAI, "far from travel target") {}

    bool IsActive() override;
};

#endif
