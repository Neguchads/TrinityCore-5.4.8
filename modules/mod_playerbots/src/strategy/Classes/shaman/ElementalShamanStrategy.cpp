#include "ElementalShamanStrategy.h"

#include "Playerbots.h"

ElementalShamanStrategy::ElementalShamanStrategy(PlayerbotAI* botAI) : CasterShamanStrategy(botAI) {}

NextAction** ElementalShamanStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("lava burst", ACTION_DEFAULT + 0.8f),
                             new NextAction("elemental blast", ACTION_DEFAULT + 0.7f),
                             new NextAction("earth shock", ACTION_DEFAULT + 0.6f),
                             new NextAction("lightning bolt", ACTION_DEFAULT + 0.5f),
                             new NextAction("chain lightning", ACTION_DEFAULT + 0.4f),
                             new NextAction("flame shock", ACTION_DEFAULT + 0.3f),
                             nullptr);
}

void ElementalShamanStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CasterShamanStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "flame shock", NextAction::array(0, new NextAction("flame shock", ACTION_HIGH + 1), nullptr)));
    triggers.push_back(new TriggerNode(
        "lava surge", NextAction::array(0, new NextAction("lava burst", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode(
        "elemental blast", NextAction::array(0, new NextAction("elemental blast", ACTION_HIGH + 1), nullptr)));
    triggers.push_back(new TriggerNode(
        "lightning shield 6", NextAction::array(0, new NextAction("earth shock", ACTION_HIGH + 1), nullptr)));
    triggers.push_back(
        new TriggerNode("ascendance", NextAction::array(0, new NextAction("ascendance", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode(
        "elemental mastery", NextAction::array(0, new NextAction("elemental mastery", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(
        new TriggerNode("no fire totem", NextAction::array(0, new NextAction("searing totem", 18.0f), nullptr)));
    triggers.push_back(new TriggerNode("fire elemental totem",
                                       NextAction::array(0, new NextAction("fire elemental totem", 32.0f), nullptr)));
}
