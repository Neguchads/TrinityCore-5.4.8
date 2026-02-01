#ifndef _PLAYERBOT_TELLTARGETACTION_H
#define _PLAYERBOT_TELLTARGETACTION_H

#include "Action.h"

class PlayerbotAI;

class TellTargetAction : public Action
{
public:
    TellTargetAction(PlayerbotAI* botAI) : Action(botAI, "tell target") {}

    bool Execute(Event event) override;
};

class TellAttackersAction : public Action
{
public:
    TellAttackersAction(PlayerbotAI* botAI) : Action(botAI, "tell attackers") {}

    bool Execute(Event event) override;
};

#endif
