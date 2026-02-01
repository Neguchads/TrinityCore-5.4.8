

#ifndef _PLAYERBOT_GENERICROGUENONCOMBATSTRATEGY_H
#define _PLAYERBOT_GENERICROGUENONCOMBATSTRATEGY_H

#include "NonCombatStrategy.h"

class PlayerbotAI;

class GenericRogueNonCombatStrategy : public NonCombatStrategy
{
public:
    GenericRogueNonCombatStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "nc"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
