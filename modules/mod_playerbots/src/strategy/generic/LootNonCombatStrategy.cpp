#include "LootNonCombatStrategy.h"

#include "Playerbots.h"

LootNonCombatStrategy::LootNonCombatStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

void LootNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("loot available", NextAction::array(0, new NextAction("loot", 6.0f), nullptr)));
    triggers.push_back(new TriggerNode("far from loot target", NextAction::array(0, new NextAction("move to loot", 7.0f), nullptr)));
    triggers.push_back(new TriggerNode("can loot", NextAction::array(0, new NextAction("open loot", 8.0f), nullptr)));
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("add all loot", 5.0f), nullptr)));
}

void GatherStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void RevealStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void UseBobberStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void RollStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void DelayedRollStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}
