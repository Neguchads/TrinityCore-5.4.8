#ifndef _PLAYERBOT_MAILACTIONS_H
#define _PLAYERBOT_MAILACTIONS_H

#include "Action.h"

class MailCommandAction : public Action
{
public:
    MailCommandAction(PlayerbotAI* botAI) : Action(botAI, "mail") {}

    bool Execute(Event event) override;
};

#endif

