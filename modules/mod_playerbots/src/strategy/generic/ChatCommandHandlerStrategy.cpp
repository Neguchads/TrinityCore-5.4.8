#include "ChatCommandHandlerStrategy.h"

#include "Playerbots.h"

ChatCommandHandlerStrategy::ChatCommandHandlerStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

void ChatCommandHandlerStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("lfg", NextAction::array(0, new NextAction("lfg", relevance), nullptr)));
    triggers.push_back(new TriggerNode("maintenance", NextAction::array(0, new NextAction("maintenance", relevance), nullptr)));
    triggers.push_back(new TriggerNode("cs", NextAction::array(0, new NextAction("cs", relevance), nullptr)));
    triggers.push_back(new TriggerNode("q", NextAction::array(0, new NextAction("query quest", relevance), nullptr)));
    triggers.push_back(new TriggerNode("quests", NextAction::array(0, new NextAction("list quests", relevance), nullptr)));
    triggers.push_back(new TriggerNode("drop", NextAction::array(0, new NextAction("drop quest", relevance), nullptr)));
    triggers.push_back(new TriggerNode("share", NextAction::array(0, new NextAction("share quest", relevance), nullptr)));
}
