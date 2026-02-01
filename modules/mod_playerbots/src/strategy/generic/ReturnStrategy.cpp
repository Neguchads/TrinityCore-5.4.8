#include "ReturnStrategy.h"

#include "Playerbots.h"

void ReturnStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "return",
        NextAction::array(0,
            new NextAction("set return position", 1.5f),
            new NextAction("return", 1.0f),
            nullptr)));
}
