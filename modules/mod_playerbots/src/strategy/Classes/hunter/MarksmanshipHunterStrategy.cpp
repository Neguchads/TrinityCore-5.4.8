#include "MarksmanshipHunterStrategy.h"

#include "Playerbots.h"

MarksmanshipHunterStrategy::MarksmanshipHunterStrategy(PlayerbotAI* botAI) : DpsHunterStrategy(botAI) {}

NextAction** MarksmanshipHunterStrategy::getDefaultActions()
{
    return NextAction::array(
        0,
        new NextAction("kill shot", ACTION_DEFAULT + 0.9f),
        new NextAction("chimera shot", ACTION_DEFAULT + 0.85f),
        new NextAction("aimed shot", ACTION_DEFAULT + 0.8f),
        new NextAction("rapid fire", ACTION_DEFAULT + 0.78f),
        new NextAction("stampede", ACTION_DEFAULT + 0.76f),
        new NextAction("a murder of crows", ACTION_DEFAULT + 0.74f),
        new NextAction("glaive toss", ACTION_DEFAULT + 0.72f),
        new NextAction("fervor", ACTION_DEFAULT + 0.7f),
        new NextAction("arcane shot", ACTION_DEFAULT + 0.6f),
        new NextAction("steady shot", ACTION_DEFAULT + 0.5f),
        new NextAction("auto shot", ACTION_DEFAULT), nullptr);
}

void MarksmanshipHunterStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericHunterStrategy::InitTriggers(triggers);

    triggers.push_back(
        new TriggerNode("fire!", NextAction::array(0, new NextAction("aimed shot", 25.0f), nullptr)));
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
