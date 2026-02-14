#ifndef _PLAYERBOT_AUCTIONACTIONS_H
#define _PLAYERBOT_AUCTIONACTIONS_H

#include "Action.h"

class AuctionCommandAction : public Action
{
public:
    AuctionCommandAction(PlayerbotAI* botAI) : Action(botAI, "ah") {}

    bool Execute(Event event) override;
};

#endif

