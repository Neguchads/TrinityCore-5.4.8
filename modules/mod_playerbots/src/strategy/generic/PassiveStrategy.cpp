#include "PassiveStrategy.h"

#include "PassiveMultiplier.h"
#include "Playerbots.h"

PassiveStrategy::PassiveStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

void PassiveStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void PassiveStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new PassiveMultiplier(botAI));
}
