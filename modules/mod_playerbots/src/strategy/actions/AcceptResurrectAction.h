#ifndef _PLAYERBOT_ACCEPTRESURRECTACTION_H
#define _PLAYERBOT_ACCEPTRESURRECTACTION_H

#include "Action.h"

class PlayerbotAI;

class AcceptResurrectAction : public Action
{
public:
    AcceptResurrectAction(PlayerbotAI* botAI) : Action(botAI, "accept resurrect") {}

    bool Execute(Event event) override;
};

#endif
