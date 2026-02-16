

#ifndef _PLAYERBOT_GENERICMAGESTRATEGY_H
#define _PLAYERBOT_GENERICMAGESTRATEGY_H

#include "CombatStrategy.h"
#include "RangedCombatStrategy.h"

class PlayerbotAI;

class GenericMageStrategy : public RangedCombatStrategy
{
public:
    GenericMageStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "mage"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    uint32 GetType() const override
    {
        return RangedCombatStrategy::GetType() | STRATEGY_TYPE_RANGED | STRATEGY_TYPE_DPS;
    }
};

class MageCcStrategy : public Strategy
{
public:
    MageCcStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}
    std::string const getName() override { return "cc"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
