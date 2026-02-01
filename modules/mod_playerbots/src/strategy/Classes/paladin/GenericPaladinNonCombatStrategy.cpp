

#include "GenericPaladinNonCombatStrategy.h"

#include "GenericPaladinStrategyActionNodeFactory.h"
#include "Playerbots.h"

GenericPaladinNonCombatStrategy::GenericPaladinNonCombatStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI)
{
    actionNodeFactories.Add(new GenericPaladinStrategyActionNodeFactory());
}

void GenericPaladinNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    NonCombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("party member dead", NextAction::array(0, new NextAction("redemption", ACTION_CRITICAL_HEAL + 10), nullptr)));
    triggers.push_back(new TriggerNode("party member almost full health", NextAction::array(0, new NextAction("flash of light on party", 25.0f), nullptr)));
    triggers.push_back(new TriggerNode("party member medium health", NextAction::array(0, new NextAction("flash of light on party", 26.0f), nullptr)));
    triggers.push_back(new TriggerNode("party member low health", NextAction::array(0, new NextAction("word of glory on party", 27.0f), nullptr)));
    triggers.push_back(new TriggerNode("party member critical health", NextAction::array(0, new NextAction("word of glory on party", 28.0f), nullptr)));
}
