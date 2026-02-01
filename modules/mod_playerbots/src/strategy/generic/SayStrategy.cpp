#include "SayStrategy.h"

#include "Playerbots.h"

SayStrategy::SayStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

void SayStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("critical health",
        NextAction::array(0, new NextAction("say::critical health", 99.0f), nullptr)));
    triggers.push_back(new TriggerNode("low health",
        NextAction::array(0, new NextAction("say::low health", 99.0f), nullptr)));
    triggers.push_back(new TriggerNode("low mana",
        NextAction::array(0, new NextAction("say::low mana", 99.0f), nullptr)));
    triggers.push_back(new TriggerNode("medium aoe",
        NextAction::array(0, new NextAction("say::aoe", 99.0f), nullptr)));
}
