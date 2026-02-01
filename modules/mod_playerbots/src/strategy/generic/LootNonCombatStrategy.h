#ifndef _PLAYERBOT_LOOTNONCOMBATSTRATEGY_H
#define _PLAYERBOT_LOOTNONCOMBATSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class LootNonCombatStrategy : public Strategy
{
public:
    LootNonCombatStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "loot"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class GatherStrategy : public Strategy
{
public:
    GatherStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "gather"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RevealStrategy : public Strategy
{
public:
    RevealStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "reveal"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class UseBobberStrategy : public Strategy
{
public:
    UseBobberStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    std::string const getName() override { return "use bobber"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RollStrategy : public Strategy
{
public:
    RollStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "roll"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class DelayedRollStrategy : public Strategy
{
public:
    DelayedRollStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "delayed roll"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
