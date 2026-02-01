#ifndef _PLAYERBOT_TANKFERALDRUIDSTRATEGY_H
#define _PLAYERBOT_TANKFERALDRUIDSTRATEGY_H

#include "BearTankDruidStrategy.h"

class PlayerbotAI;

class TankFeralDruidStrategy : public BearTankDruidStrategy
{
public:
    TankFeralDruidStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "tank feral"; }
};

#endif
