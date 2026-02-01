#include "ProtectionPaladinStrategy.h"

#include "Playerbots.h"

ProtectionPaladinStrategy::ProtectionPaladinStrategy(PlayerbotAI* botAI) : TankPaladinStrategy(botAI) {}

NextAction** ProtectionPaladinStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("avenger's shield", ACTION_DEFAULT + 0.9f),
                             new NextAction("shield of righteousness", ACTION_DEFAULT + 0.8f),
                             new NextAction("crusader strike", ACTION_DEFAULT + 0.7f),
                             new NextAction("hammer of the righteous", ACTION_DEFAULT + 0.65f),
                             new NextAction("judgement", ACTION_DEFAULT + 0.6f),
                             new NextAction("consecration", ACTION_DEFAULT + 0.5f),
                             new NextAction("holy wrath", ACTION_DEFAULT + 0.4f),
                             new NextAction("hammer of wrath", ACTION_DEFAULT + 0.3f),
                             new NextAction("melee", ACTION_DEFAULT), nullptr);
}

void ProtectionPaladinStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericPaladinStrategy::InitTriggers(triggers);

    triggers.push_back(
        new TriggerNode("seal", NextAction::array(0, new NextAction("seal of truth", ACTION_HIGH), nullptr)));
    triggers.push_back(
        new TriggerNode("light aoe", NextAction::array(0, new NextAction("seal of righteousness", ACTION_HIGH + 1), nullptr)));

    triggers.push_back(
        new TriggerNode("grand crusader", NextAction::array(0, new NextAction("avenger's shield", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(
        new TriggerNode("holy power", NextAction::array(0, new NextAction("shield of righteousness", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(
        new TriggerNode("bastion of glory 5", NextAction::array(0, new NextAction("eternal flame", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(
        new TriggerNode("critical health", NextAction::array(0, new NextAction("ardent defender", ACTION_EMERGENCY + 6),
                                                            new NextAction("guardian of ancient kings", ACTION_EMERGENCY + 5),
                                                            new NextAction("divine protection", ACTION_EMERGENCY + 4),
                                                            nullptr)));

    triggers.push_back(
        new TriggerNode("lose aggro", NextAction::array(0, new NextAction("hand of reckoning", ACTION_HIGH + 7), nullptr)));
    triggers.push_back(new TriggerNode(
        "target critical health",
        NextAction::array(0, new NextAction("hammer of wrath", ACTION_HIGH + 6), nullptr)));
    triggers.push_back(new TriggerNode(
        "enemy out of melee", NextAction::array(0, new NextAction("reach melee", ACTION_HIGH + 1), nullptr)));
}
