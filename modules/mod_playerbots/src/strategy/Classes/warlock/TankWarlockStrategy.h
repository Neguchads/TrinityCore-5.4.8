#ifndef _PLAYERBOT_TANKWARLOCKSTRATEGY_H
#define _PLAYERBOT_TANKWARLOCKSTRATEGY_H

#include "GenericWarlockStrategy.h"

class PlayerbotAI;

class TankWarlockStrategy : public GenericWarlockStrategy
{
public:
    TankWarlockStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "tank"; }
    uint32 GetType() const override { return GenericWarlockStrategy::GetType() | STRATEGY_TYPE_TANK; }
};

#endif
