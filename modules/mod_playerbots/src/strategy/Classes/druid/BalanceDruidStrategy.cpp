#include "BalanceDruidStrategy.h"

#include "GenericDruidStrategy.h"
#include "Playerbots.h"

BalanceDruidStrategy::BalanceDruidStrategy(PlayerbotAI* botAI) : CasterDruidStrategy(botAI) {}

NextAction** BalanceDruidStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("starfall", ACTION_DEFAULT + 0.9f),
        new NextAction("celestial alignment", ACTION_DEFAULT + 0.8f),
        new NextAction("force of nature", ACTION_DEFAULT + 0.7f),
        new NextAction("starsurge", ACTION_DEFAULT + 0.6f),
        new NextAction("sunfire", ACTION_DEFAULT + 0.5f),
        new NextAction("moonfire", ACTION_DEFAULT + 0.4f),
        new NextAction("starfire", ACTION_DEFAULT + 0.3f),
        new NextAction("wrath", ACTION_DEFAULT + 0.2f),
        nullptr);
}

void BalanceDruidStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CasterDruidStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "starfall", NextAction::array(0, new NextAction("starfall", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(new TriggerNode(
        "celestial alignment", NextAction::array(0, new NextAction("celestial alignment", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(new TriggerNode(
        "force of nature", NextAction::array(0, new NextAction("force of nature", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode(
        "astral communion", NextAction::array(0, new NextAction("astral communion", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode(
        "shooting stars", NextAction::array(0, new NextAction("starsurge", ACTION_HIGH + 6), nullptr)));
    triggers.push_back(new TriggerNode(
        "starsurge", NextAction::array(0, new NextAction("starsurge", ACTION_HIGH + 2), nullptr)));

    triggers.push_back(new TriggerNode(
        "sunfire", NextAction::array(0, new NextAction("sunfire", ACTION_NORMAL + 5), nullptr)));
    triggers.push_back(new TriggerNode(
        "moonfire", NextAction::array(0, new NextAction("moonfire", ACTION_NORMAL + 4), nullptr)));

    triggers.push_back(new TriggerNode("eclipse (lunar) cooldown",
                                       NextAction::array(0, new NextAction("starfire", ACTION_DEFAULT + 0.2f), nullptr)));
    triggers.push_back(new TriggerNode("eclipse (solar) cooldown",
                                       NextAction::array(0, new NextAction("wrath", ACTION_DEFAULT + 0.2f), nullptr)));

    triggers.push_back(
        new TriggerNode("eclipse (solar)", NextAction::array(0, new NextAction("wrath", ACTION_NORMAL + 6), nullptr)));
    triggers.push_back(new TriggerNode("eclipse (lunar)",
                                       NextAction::array(0, new NextAction("starfire", ACTION_NORMAL + 6), nullptr)));

    triggers.push_back(new TriggerNode("medium mana",
        NextAction::array(0, new NextAction("innervate", ACTION_HIGH + 9), nullptr)));
    triggers.push_back(new TriggerNode("enemy too close for spell",
        NextAction::array(0,
            new NextAction("typhoon", ACTION_HIGH + 3),
            new NextAction("flee", ACTION_MOVE + 9),
            nullptr)));

    triggers.push_back(new TriggerNode("critical health",
        NextAction::array(0, new NextAction("barkskin", ACTION_EMERGENCY + 2), nullptr)));
    triggers.push_back(new TriggerNode("low health",
        NextAction::array(0, new NextAction("might of ursoc", ACTION_EMERGENCY + 1), nullptr)));
}
