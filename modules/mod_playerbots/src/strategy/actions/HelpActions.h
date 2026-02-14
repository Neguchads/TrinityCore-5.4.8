#ifndef _PLAYERBOT_HELPACTIONS_H
#define _PLAYERBOT_HELPACTIONS_H

#include "Action.h"

class HelpCommandAction : public Action
{
public:
    HelpCommandAction(PlayerbotAI* botAI) : Action(botAI, "help") {}

    bool Execute(Event event) override;
};

#endif

