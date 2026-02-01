#include "RestoShamanStrategy.h"

#include "Playerbots.h"

RestoShamanStrategy::RestoShamanStrategy(PlayerbotAI* botAI) : HealShamanStrategy(botAI) {}

NextAction** RestoShamanStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("riptide", ACTION_DEFAULT + 0.4f),
                             new NextAction("healing wave", ACTION_DEFAULT + 0.3f),
                             new NextAction("healing surge", ACTION_DEFAULT + 0.2f),
                             nullptr);
}

void RestoShamanStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    HealShamanStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "group heal setting",
        NextAction::array(0, new NextAction("healing rain", 30.0f), new NextAction("chain heal on party", 28.0f),
                          nullptr)));

    triggers.push_back(new TriggerNode(
        "party member critical health",
        NextAction::array(0, new NextAction("healing surge on party", 27.0f),
                          new NextAction("riptide on party", 26.0f),
                          new NextAction("healing wave on party", 25.0f), nullptr)));

    triggers.push_back(new TriggerNode(
        "party member low health",
        NextAction::array(0, new NextAction("riptide on party", 23.0f),
                          new NextAction("healing surge on party", 22.0f),
                          new NextAction("healing wave on party", 21.0f), nullptr)));

    triggers.push_back(new TriggerNode(
        "party member medium health",
        NextAction::array(0, new NextAction("riptide on party", 19.0f),
                          new NextAction("healing wave on party", 18.0f), nullptr)));

    triggers.push_back(new TriggerNode(
        "party member almost full health",
        NextAction::array(0, new NextAction("riptide on party", 16.0f), nullptr)));

    triggers.push_back(
        new TriggerNode("earth shield on main tank",
                        NextAction::array(0, new NextAction("earth shield on main tank", ACTION_HIGH + 7), nullptr)));

    triggers.push_back(new TriggerNode("healing tide totem",
                                       NextAction::array(0, new NextAction("healing tide totem", ACTION_HIGH + 6),
                                                         nullptr)));
    triggers.push_back(new TriggerNode("spirit link totem",
                                       NextAction::array(0, new NextAction("spirit link totem", ACTION_HIGH + 5),
                                                         nullptr)));
}
