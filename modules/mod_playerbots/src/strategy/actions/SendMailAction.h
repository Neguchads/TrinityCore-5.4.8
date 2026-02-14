#ifndef _PLAYERBOT_SENDMAILACTION_H
#define _PLAYERBOT_SENDMAILACTION_H

#include "Action.h"

// Chat command alias from CMaNGOS: "sendmail ..."
// Forwards into the existing "mail send ..." implementation.
class SendMailAction : public Action
{
public:
    SendMailAction(PlayerbotAI* botAI) : Action(botAI, "sendmail") {}

    bool Execute(Event event) override;
};

#endif

