#include "EnhancementShamanStrategy.h"

#include "Playerbots.h"

EnhancementShamanStrategy::EnhancementShamanStrategy(PlayerbotAI* botAI) : MeleeShamanStrategy(botAI) {}

NextAction** EnhancementShamanStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("stormstrike", ACTION_DEFAULT + 0.8f),
                             new NextAction("lava lash", ACTION_DEFAULT + 0.7f),
                             new NextAction("unleash elements", ACTION_DEFAULT + 0.6f),
                             new NextAction("earth shock", ACTION_DEFAULT + 0.5f),
                             new NextAction("flame shock", ACTION_DEFAULT + 0.4f),
                             new NextAction("melee", ACTION_DEFAULT),
                             nullptr);
}

void EnhancementShamanStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MeleeShamanStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "maelstrom weapon 5",
        NextAction::array(0, new NextAction("elemental blast", ACTION_HIGH + 1),
                          new NextAction("lightning bolt", ACTION_HIGH), nullptr)));
    triggers.push_back(
        new TriggerNode("ascendance", NextAction::array(0, new NextAction("ascendance", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode(
        "elemental mastery", NextAction::array(0, new NextAction("elemental mastery", ACTION_HIGH + 1), nullptr)));
    triggers.push_back(new TriggerNode("fire elemental totem",
                                       NextAction::array(0, new NextAction("fire elemental totem", 32.0f), nullptr)));
}
