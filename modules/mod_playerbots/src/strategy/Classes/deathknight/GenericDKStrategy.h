

#ifndef _PLAYERBOT_GENERICDKSTRATEGY_H
#define _PLAYERBOT_GENERICDKSTRATEGY_H

#include "MeleeCombatStrategy.h"

class PlayerbotAI;

class GenericDKStrategy : public MeleeCombatStrategy
{
public:
    GenericDKStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "DK"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class DKCcStrategy : public Strategy
{
public:
    DKCcStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "cc"; }
};

#endif
