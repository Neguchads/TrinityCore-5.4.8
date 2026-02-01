

#ifndef _PLAYERBOT_GENERICDKNONCOMBATSTRATEGY_H
#define _PLAYERBOT_GENERICDKNONCOMBATSTRATEGY_H

#include "GenericDKStrategy.h"
#include "NonCombatStrategy.h"

class PlayerbotAI;

class GenericDKNonCombatStrategy : public NonCombatStrategy
{
public:
    GenericDKNonCombatStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "nc"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class DKBuffDpsStrategy : public Strategy
{
public:
    DKBuffDpsStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "bdps"; }
};

#endif
