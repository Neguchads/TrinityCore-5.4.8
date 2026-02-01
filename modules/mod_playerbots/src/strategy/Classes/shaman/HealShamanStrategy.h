

#ifndef _PLAYERBOT_HEALSHAMANSTRATEGY_H
#define _PLAYERBOT_HEALSHAMANSTRATEGY_H

#include "GenericShamanStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

class HealShamanStrategy : public GenericShamanStrategy
{
public:
    HealShamanStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "heal"; }
    uint32 GetType() const override { return STRATEGY_TYPE_RANGED | STRATEGY_TYPE_HEAL; }
};

#endif
