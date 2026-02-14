#ifndef _PLAYERBOT_BANKACTIONS_H
#define _PLAYERBOT_BANKACTIONS_H

#include "Action.h"

class BankCommandAction : public Action
{
public:
    BankCommandAction(PlayerbotAI* botAI) : Action(botAI, "bank") {}

    bool Execute(Event event) override;
};

#endif

