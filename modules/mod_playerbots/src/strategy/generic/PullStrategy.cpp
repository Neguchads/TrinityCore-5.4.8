#include "PullStrategy.h"

#include "Playerbots.h"

PullStrategy::PullStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

void PullStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void PossibleAddsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void PullBackStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}
