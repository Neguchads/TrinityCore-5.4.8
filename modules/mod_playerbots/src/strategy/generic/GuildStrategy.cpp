#include "GuildStrategy.h"

#include "Playerbots.h"

GuildStrategy::GuildStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

void GuildStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("random", NextAction::array(0, new NextAction("offer petition nearby", 4.0f), nullptr)));
    triggers.push_back(new TriggerNode("random", NextAction::array(0, new NextAction("guild manage nearby", 4.0f), nullptr)));
    triggers.push_back(new TriggerNode("petition signed", NextAction::array(0, new NextAction("turn in petition", 10.0f), nullptr)));
    triggers.push_back(new TriggerNode("buy tabard", NextAction::array(0, new NextAction("buy tabard", 10.0f), nullptr)));
    triggers.push_back(new TriggerNode("leave large guild", NextAction::array(0, new NextAction("guild leave", 4.0f), nullptr)));
}
