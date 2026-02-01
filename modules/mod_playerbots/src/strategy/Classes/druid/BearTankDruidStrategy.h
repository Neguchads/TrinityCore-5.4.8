

#ifndef _PLAYERBOT_BEARTANKDRUIDSTRATEGY_H
#define _PLAYERBOT_BEARTANKDRUIDSTRATEGY_H

#include "FeralDruidStrategy.h"

class PlayerbotAI;

class BearTankDruidStrategy : public FeralDruidStrategy
{
public:
    BearTankDruidStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "bear"; }
    NextAction** getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
};

#endif
