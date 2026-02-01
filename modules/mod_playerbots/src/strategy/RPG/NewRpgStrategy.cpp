

#include "NewRpgStrategy.h"

#include "Playerbots.h"

NewRpgStrategy::NewRpgStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

NextAction** NewRpgStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("new rpg status update", 5.0f), nullptr);
}

void NewRpgStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("go grind status", NextAction::array(0, new NextAction("new rpg go grind", 1.0f), nullptr)));
    
    triggers.push_back(
        new TriggerNode("go innkeeper status", NextAction::array(0, new NextAction("new rpg go innkeeper", 1.0f), nullptr)));

    triggers.push_back(
        new TriggerNode("near random status", NextAction::array(0, new NextAction("new rpg move random", 1.0f), nullptr)));

    triggers.push_back(
        new TriggerNode("near npc status", NextAction::array(0, new NextAction("new rpg move npc", 1.0f), nullptr)));
}

void NewRpgStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    // multipliers.push_back(new RpgActionMultiplier(botAI));
}
