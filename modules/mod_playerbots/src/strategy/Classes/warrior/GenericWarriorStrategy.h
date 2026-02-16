#ifndef _PLAYERBOT_GENERICWARRIORSTRATEGY_H
#define _PLAYERBOT_GENERICWARRIORSTRATEGY_H

#include "CombatStrategy.h"

class PlayerbotAI;

class GenericWarriorStrategy : public CombatStrategy
{
public:
    GenericWarriorStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "warrior"; }
};

class WarriorAoeStrategy : public CombatStrategy
{
public:
    WarriorAoeStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "aoe"; }
};

class WarriorCcStrategy : public Strategy
{
public:
    WarriorCcStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "cc"; }
};

#endif
