

#ifndef _PLAYERBOT_GENERICHUNTERNONCOMBATSTRATEGY_H
#define _PLAYERBOT_GENERICHUNTERNONCOMBATSTRATEGY_H

#include "NonCombatStrategy.h"

class PlayerbotAI;

class GenericHunterNonCombatStrategy : public NonCombatStrategy
{
public:
    GenericHunterNonCombatStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "nc"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class HunterPetStrategy : public Strategy
{
public:
    HunterPetStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "pet"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
