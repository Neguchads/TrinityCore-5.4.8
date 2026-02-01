#include "GuardStrategy.h"

#include "Playerbots.h"

NextAction** GuardStrategy::getDefaultActions() { return NextAction::array(0, new NextAction("guard", 4.0f), nullptr); }

void GuardStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("no target", NextAction::array(0, new NextAction("guard", 5.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("guard", 4.0f), nullptr)));
}
