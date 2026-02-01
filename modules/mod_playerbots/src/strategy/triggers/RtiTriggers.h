#ifndef _PLAYERBOT_RTITRIGGERS_H
#define _PLAYERBOT_RTITRIGGERS_H

#include "Trigger.h"

class PlayerbotAI;

class NoRtiTrigger : public Trigger
{
public:
    NoRtiTrigger(PlayerbotAI* botAI) : Trigger(botAI, "no rti target") {}

    bool IsActive() override;
};

#endif
