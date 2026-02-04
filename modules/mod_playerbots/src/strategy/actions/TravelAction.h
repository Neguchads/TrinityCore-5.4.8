#ifndef _PLAYERBOT_TRAVELACTIONS_H
#define _PLAYERBOT_TRAVELACTIONS_H

#include "MovementActions.h"

class PlayerbotAI;

class TravelAction : public MovementAction
{
public:
    TravelAction(PlayerbotAI* botAI) : MovementAction(botAI, "travel") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

#endif
