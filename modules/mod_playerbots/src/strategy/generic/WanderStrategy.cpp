#include "WanderStrategy.h"

#include "Playerbots.h"

void WanderStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("move random", 1.5f), nullptr)));
}
