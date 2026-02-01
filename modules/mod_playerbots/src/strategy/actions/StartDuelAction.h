#ifndef _PLAYERBOT_STARTDUELACTION_H
#define _PLAYERBOT_STARTDUELACTION_H

#include "Action.h"

class PlayerbotAI;
class Player;

class StartDuelAction : public Action
{
public:
    StartDuelAction(PlayerbotAI* botAI) : Action(botAI, "start duel") {}

    bool Execute(Event event) override;
    bool isUseful() override;

private:
    Player* SelectOpponent();
};

#endif
