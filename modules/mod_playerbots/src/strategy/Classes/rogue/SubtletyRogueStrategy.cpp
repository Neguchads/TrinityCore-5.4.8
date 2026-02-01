#include "SubtletyRogueStrategy.h"

#include "Playerbots.h"

SubtletyRogueStrategy::SubtletyRogueStrategy(PlayerbotAI* botAI) : DpsRogueStrategy(botAI) {}

NextAction** SubtletyRogueStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("premeditation", ACTION_DEFAULT + 0.9f),
                             new NextAction("shadow dance", ACTION_DEFAULT + 0.85f),
                             new NextAction("shadow blades", ACTION_DEFAULT + 0.8f),
                             new NextAction("slice and dice", ACTION_DEFAULT + 0.7f),
                             new NextAction("rupture", ACTION_DEFAULT + 0.6f),
                             new NextAction("eviscerate", ACTION_DEFAULT + 0.5f),
                             new NextAction("backstab", ACTION_DEFAULT + 0.4f),
                             new NextAction("hemorrhage", ACTION_DEFAULT + 0.3f),
                             new NextAction("melee", ACTION_DEFAULT), nullptr);
}

void SubtletyRogueStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    DpsRogueStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("slice and dice",
                                       NextAction::array(0, new NextAction("slice and dice", ACTION_HIGH + 6), nullptr)));
    triggers.push_back(new TriggerNode("rupture",
                                       NextAction::array(0, new NextAction("rupture", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(new TriggerNode("hemorrhage",
                                       NextAction::array(0, new NextAction("hemorrhage", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(
        new TriggerNode("behind target", NextAction::array(0, new NextAction("backstab", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode(
        "not behind target", NextAction::array(0, new NextAction("hemorrhage", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode(
        "medium aoe", NextAction::array(0, new NextAction("fan of knives", ACTION_NORMAL + 6),
                                        new NextAction("crimson tempest", ACTION_NORMAL + 5), nullptr)));
}
