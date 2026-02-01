#include "RetributionPaladinStrategy.h"

#include "Playerbots.h"

RetributionPaladinStrategy::RetributionPaladinStrategy(PlayerbotAI* botAI) : DpsPaladinStrategy(botAI) {}

NextAction** RetributionPaladinStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("avenging wrath", ACTION_DEFAULT + 0.9f),
                             new NextAction("guardian of ancient kings", ACTION_DEFAULT + 0.85f),
                             new NextAction("holy avenger", ACTION_DEFAULT + 0.84f),
                             new NextAction("execution sentence", ACTION_DEFAULT + 0.8f),
                             new NextAction("hammer of wrath", ACTION_DEFAULT + 0.75f),
                             new NextAction("templar's verdict", ACTION_DEFAULT + 0.7f),
                             new NextAction("exorcism", ACTION_DEFAULT + 0.6f),
                             new NextAction("crusader strike", ACTION_DEFAULT + 0.5f),
                             new NextAction("judgement", ACTION_DEFAULT + 0.4f),
                             new NextAction("divine storm", ACTION_DEFAULT + 0.3f),
                             new NextAction("melee", ACTION_DEFAULT), nullptr);
}

void RetributionPaladinStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericPaladinStrategy::InitTriggers(triggers);

    triggers.push_back(
        new TriggerNode("seal", NextAction::array(0, new NextAction("seal of truth", ACTION_HIGH), nullptr)));
    triggers.push_back(
        new TriggerNode("light aoe", NextAction::array(0, new NextAction("seal of righteousness", ACTION_HIGH + 1), nullptr)));

    triggers.push_back(
        new TriggerNode("inquisition", NextAction::array(0, new NextAction("inquisition", ACTION_HIGH + 6), nullptr)));
    triggers.push_back(
        new TriggerNode("the art of war", NextAction::array(0, new NextAction("exorcism", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(
        new TriggerNode("holy power", NextAction::array(0, new NextAction("templar's verdict", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(
        new TriggerNode("divine purpose", NextAction::array(0, new NextAction("templar's verdict", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(
        new TriggerNode("target critical health", NextAction::array(0, new NextAction("hammer of wrath", ACTION_HIGH + 7), nullptr)));
    triggers.push_back(new TriggerNode(
        "enemy out of melee", NextAction::array(0, new NextAction("reach melee", ACTION_HIGH + 1), nullptr)));
}
