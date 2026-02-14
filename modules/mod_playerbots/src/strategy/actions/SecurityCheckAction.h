#ifndef _PLAYERBOT_SECURITYCHECKACTION_H
#define _PLAYERBOT_SECURITYCHECKACTION_H

#include "Action.h"

// Ported from CMaNGOS: ensures random bots don't get abused via loot rules unless guildmates.
class SecurityCheckAction : public Action
{
public:
    SecurityCheckAction(PlayerbotAI* botAI) : Action(botAI, "security check") {}

    bool isUseful() override;
    bool Execute(Event event) override;
    bool isUsefulWhenStunned() override { return true; }
};

#endif

