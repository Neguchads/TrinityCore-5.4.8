#include "ChatCommandHandlerStrategy.h"

#include "Playerbots.h"

ChatCommandHandlerStrategy::ChatCommandHandlerStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

void ChatCommandHandlerStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("lfg", NextAction::array(0, new NextAction("lfg", relevance), nullptr)));
    triggers.push_back(new TriggerNode("maintenance", NextAction::array(0, new NextAction("maintenance", relevance), nullptr)));
}
