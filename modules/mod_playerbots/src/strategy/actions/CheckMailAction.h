#ifndef _PLAYERBOT_CHECKMAILACTION_H
#define _PLAYERBOT_CHECKMAILACTION_H

#include "Action.h"

struct Mail;

// Ported from CMaNGOS playerbots:
// Automatically checks bot mailbox and returns "mistaken" items to sender when the bot has no active master.
class CheckMailAction : public Action
{
public:
    CheckMailAction(PlayerbotAI* botAI) : Action(botAI, "check mail") {}

    bool Execute(Event event) override;
    bool isUseful() override;

private:
    void ProcessMail(Mail* mail, uint32 senderLow);
};

#endif
