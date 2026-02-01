

#ifndef _PLAYERBOT_COMBATSTRATEGY_H
#define _PLAYERBOT_COMBATSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class CombatStrategy : public Strategy
{
public:
    CombatStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT; }
};

class AvoidAoeStrategy : public Strategy
{
public:
    explicit AvoidAoeStrategy(PlayerbotAI* ai);
    const std::string getName() override { return "avoid aoe"; }
    NextAction** getDefaultActions() override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class TankFaceStrategy : public Strategy
{
public:
    explicit TankFaceStrategy(PlayerbotAI* ai);
    const std::string getName() override { return "tank face"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class CombatFormationStrategy : public Strategy
{
public:
    CombatFormationStrategy(PlayerbotAI* ai) : Strategy(ai) {}
    const std::string getName() override { return "formation"; }
    NextAction** getDefaultActions() override;
};

class WaitForAttackStrategy : public Strategy
{
public:
    WaitForAttackStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    std::string const getName() override { return "wait for attack"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class HealInterruptStrategy : public Strategy
{
public:
    HealInterruptStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    std::string const getName() override { return "heal interrupt"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class PreHealStrategy : public Strategy
{
public:
    PreHealStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    std::string const getName() override { return "preheal"; }
};

#endif
