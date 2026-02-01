

#ifndef _PLAYERBOT_TANKPALADINSTRATEGY_H
#define _PLAYERBOT_TANKPALADINSTRATEGY_H

#include "GenericPaladinStrategy.h"

class PlayerbotAI;

class TankPaladinStrategy : public GenericPaladinStrategy
{
public:
    TankPaladinStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "tank"; }
    NextAction** getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
};

#endif
