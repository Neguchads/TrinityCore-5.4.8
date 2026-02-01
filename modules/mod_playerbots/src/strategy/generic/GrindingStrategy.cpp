#include "GrindingStrategy.h"

#include "Playerbots.h"

NextAction** GrindingStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("drink", 4.2f),
        new NextAction("food", 4.1f),
        nullptr);
}

void GrindingStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("no target", NextAction::array(0, new NextAction("attack anything", 4.0f), nullptr)));
}

void MoveRandomStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("move random", 1.5f), nullptr)));
}
