#ifndef _PLAYERBOT_CHECKMOUNTSTATEACTION_H
#define _PLAYERBOT_CHECKMOUNTSTATEACTION_H

#include "Action.h"

class CheckMountStateAction : public Action
{
public:
    CheckMountStateAction(PlayerbotAI* botAI) : Action(botAI, "check mount state") {}

    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override { return true; }

private:
    bool Mount();
    void Dismount();
};

#endif
