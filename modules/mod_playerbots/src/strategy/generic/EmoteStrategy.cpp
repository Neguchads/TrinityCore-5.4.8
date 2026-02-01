#include "EmoteStrategy.h"

#include "Playerbots.h"

EmoteStrategy::EmoteStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

void EmoteStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    if (sPlayerbotAIConfig->randomBotTalk)
        triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("talk", 1.0f), nullptr)));

    if (sPlayerbotAIConfig->randomBotEmote)
        triggers.push_back(new TriggerNode("seldom", NextAction::array(0, new NextAction("emote", 1.0f), nullptr)));

    if (sPlayerbotAIConfig->enableGreet)
        triggers.push_back(new TriggerNode("new player nearby", NextAction::array(0, new NextAction("greet", 1.0f), nullptr)));
}
