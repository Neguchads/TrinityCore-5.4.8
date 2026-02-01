#include "UseFoodStrategy.h"

#include "PlayerbotAIConfig.h"
#include "Playerbots.h"

void UseFoodStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    Strategy::InitTriggers(triggers);
    if (botAI->HasCheat(BotCheatMask::food))
    {
        triggers.push_back(new TriggerNode("medium health", NextAction::array(0, new NextAction("food", 3.0f), nullptr)));
        triggers.push_back(new TriggerNode("high mana", NextAction::array(0, new NextAction("drink", 3.0f), nullptr)));
    }
    else
    {
        triggers.push_back(new TriggerNode("low health", NextAction::array(0, new NextAction("food", 3.0f), nullptr)));
        triggers.push_back(new TriggerNode("low mana", NextAction::array(0, new NextAction("drink", 3.0f), nullptr)));
    }
}
