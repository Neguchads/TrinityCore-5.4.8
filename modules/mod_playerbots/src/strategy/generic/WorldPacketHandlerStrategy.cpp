

#include "WorldPacketHandlerStrategy.h"

#include "Playerbots.h"

void WorldPacketHandlerStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    PassTroughStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("levelup", NextAction::array(0, new NextAction("auto maintenance on levelup", relevance + 3), nullptr)));

    // party handlers
    triggers.push_back(new TriggerNode("group invite", NextAction::array(0, new NextAction("accept invitation", relevance), nullptr)));
    triggers.push_back(new TriggerNode("uninvite guid", NextAction::array(0, new NextAction("uninvite", relevance), nullptr)));
    triggers.push_back(new TriggerNode("uninvite", NextAction::array(0, new NextAction("uninvite", relevance), nullptr)));

    triggers.push_back(new TriggerNode("release spirit", NextAction::array(0, new NextAction("release", relevance), nullptr)));
    triggers.push_back(new TriggerNode("revive from corpse", NextAction::array(0, new NextAction("revive from corpse", relevance), nullptr)));

    triggers.push_back(new TriggerNode("duel requested", NextAction::array(0, new NextAction("accept duel", relevance), nullptr)));
    triggers.push_back(new TriggerNode("lfg proposal", NextAction::array(0, new NextAction("lfg accept", relevance), nullptr)));
    triggers.push_back(new TriggerNode("lfg proposal active", NextAction::array(0, new NextAction("lfg accept", relevance), nullptr)));
    triggers.push_back(new TriggerNode("lfg teleport", NextAction::array(0, new NextAction("lfg teleport", relevance), nullptr)));
}

WorldPacketHandlerStrategy::WorldPacketHandlerStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI)
{
    supported.push_back("uninvite");
    supported.push_back("lfg role check");
    supported.push_back("lfg teleport");
    supported.push_back("bg status");
    supported.push_back("check mount state");
}

void ReadyCheckStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    PassTroughStrategy::InitTriggers(triggers);
}
