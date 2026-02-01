

#ifndef _PLAYERBOT_GENERICPALADINNONCOMBATSTRATEGY_H
#define _PLAYERBOT_GENERICPALADINNONCOMBATSTRATEGY_H

#include "NonCombatStrategy.h"

class PlayerbotAI;

class GenericPaladinNonCombatStrategy : public NonCombatStrategy
{
public:
    GenericPaladinNonCombatStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "nc"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
