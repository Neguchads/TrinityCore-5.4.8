#include "SurvivalHunterStrategy.h"

#include "Playerbots.h"

SurvivalHunterStrategy::SurvivalHunterStrategy(PlayerbotAI* botAI) : DpsHunterStrategy(botAI) {}

NextAction** SurvivalHunterStrategy::getDefaultActions()
{
    return NextAction::array(
        0,
        new NextAction("explosive shot", ACTION_DEFAULT + 0.9f),
        new NextAction("black arrow", ACTION_DEFAULT + 0.85f),
        new NextAction("kill shot", ACTION_DEFAULT + 0.82f),
        new NextAction("stampede", ACTION_DEFAULT + 0.8f),
        new NextAction("a murder of crows", ACTION_DEFAULT + 0.78f),
        new NextAction("glaive toss", ACTION_DEFAULT + 0.76f),
        new NextAction("fervor", ACTION_DEFAULT + 0.74f),
        new NextAction("arcane shot", ACTION_DEFAULT + 0.6f),
        new NextAction("cobra shot", ACTION_DEFAULT + 0.5f),
        new NextAction("auto shot", ACTION_DEFAULT), nullptr);
}

void SurvivalHunterStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericHunterStrategy::InitTriggers(triggers);

    triggers.push_back(
        new TriggerNode("lock and load", NextAction::array(0, new NextAction("explosive shot", 25.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("black arrow", NextAction::array(0, new NextAction("black arrow", 23.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("serpent sting", NextAction::array(0, new NextAction("serpent sting", 21.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("hunter's mark", NextAction::array(0, new NextAction("hunter's mark", 20.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("target critical health", NextAction::array(0, new NextAction("kill shot", 19.0f), nullptr)));
    triggers.push_back(new TriggerNode("concussive shot on snare target",
                                       NextAction::array(0, new NextAction("concussive shot", 18.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("has aggro", NextAction::array(0, new NextAction("concussive shot", 17.0f), nullptr)));
}
