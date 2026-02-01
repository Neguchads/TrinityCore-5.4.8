

#ifndef _PLAYERBOT_GENERICHUNTERSTRATEGY_H
#define _PLAYERBOT_GENERICHUNTERSTRATEGY_H

#include "CombatStrategy.h"
#include "Strategy.h"

class PlayerbotAI;

class GenericHunterStrategy : public CombatStrategy
{
public:
    GenericHunterStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "hunter"; }
    uint32 GetType() const override { return CombatStrategy::GetType() | STRATEGY_TYPE_RANGED | STRATEGY_TYPE_DPS; }
};

class HunterBoostStrategy : public Strategy
{
public:
    HunterBoostStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "boost"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class HunterCcStrategy : public Strategy
{
public:
    HunterCcStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "cc"; }
};

#endif
