#ifndef _PLAYERBOT_ACCEPTDUELACTION_H
#define _PLAYERBOT_ACCEPTDUELACTION_H

#include "Action.h"

class PlayerbotAI;

class AcceptDuelAction : public Action
{
public:
    AcceptDuelAction(PlayerbotAI* botAI) : Action(botAI, "accept duel") {}

    bool Execute(Event event) override;
};

#endif
