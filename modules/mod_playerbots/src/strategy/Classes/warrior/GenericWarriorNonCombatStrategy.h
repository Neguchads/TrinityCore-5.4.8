

#ifndef _PLAYERBOT_GENERICWARRIORNONCOBATSTRATEGY_H
#define _PLAYERBOT_GENERICWARRIORNONCOBATSTRATEGY_H

#include "NonCombatStrategy.h"

class PlayerbotAI;

class GenericWarriorNonCombatStrategy : public NonCombatStrategy
{
public:
    GenericWarriorNonCombatStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "nc"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
