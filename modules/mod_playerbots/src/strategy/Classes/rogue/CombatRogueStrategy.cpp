#include "CombatRogueStrategy.h"

#include "Playerbots.h"

CombatRogueStrategy::CombatRogueStrategy(PlayerbotAI* botAI) : DpsRogueStrategy(botAI) {}

NextAction** CombatRogueStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("slice and dice", ACTION_DEFAULT + 0.7f),
                             new NextAction("revealing strike", ACTION_DEFAULT + 0.6f),
                             new NextAction("rupture", ACTION_DEFAULT + 0.5f),
                             new NextAction("eviscerate", ACTION_DEFAULT + 0.4f),
                             new NextAction("sinister strike", ACTION_DEFAULT + 0.3f),
                             new NextAction("melee", ACTION_DEFAULT), nullptr);
}

void CombatRogueStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    DpsRogueStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("slice and dice",
                                       NextAction::array(0, new NextAction("slice and dice", ACTION_HIGH + 6), nullptr)));
    triggers.push_back(new TriggerNode("revealing strike",
                                       NextAction::array(0, new NextAction("revealing strike", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(new TriggerNode("rupture",
                                       NextAction::array(0, new NextAction("rupture", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(new TriggerNode("adrenaline rush",
                                       NextAction::array(0, new NextAction("adrenaline rush", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode("killing spree",
                                       NextAction::array(0, new NextAction("killing spree", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode("shadow blades",
                                       NextAction::array(0, new NextAction("shadow blades", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode("blade flurry",
                                       NextAction::array(0, new NextAction("blade flurry", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode("light aoe",
                                       NextAction::array(0, new NextAction("blade flurry", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode("medium aoe",
                                       NextAction::array(0,
                                                         new NextAction("fan of knives", ACTION_NORMAL + 6),
                                                         new NextAction("crimson tempest", ACTION_NORMAL + 5),
                                                         nullptr)));
}
