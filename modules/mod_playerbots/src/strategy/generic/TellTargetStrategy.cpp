#include "TellTargetStrategy.h"

#include "Playerbots.h"

TellTargetStrategy::TellTargetStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

void TellTargetStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("target changed", NextAction::array(0, new NextAction("tell target", 51.0f), nullptr)));
}
