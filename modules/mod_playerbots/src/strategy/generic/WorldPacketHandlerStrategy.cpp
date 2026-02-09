

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
    triggers.push_back(new TriggerNode("confirm quest", NextAction::array(0, new NextAction("confirm quest", relevance), nullptr)));
    triggers.push_back(new TriggerNode("quest update add kill", NextAction::array(0, new NextAction("quest update add kill", relevance), nullptr)));
    triggers.push_back(new TriggerNode("quest update failed", NextAction::array(0, new NextAction("quest update failed", relevance), nullptr)));
    triggers.push_back(new TriggerNode("quest update failed timer", NextAction::array(0, new NextAction("quest update failed timer", relevance), nullptr)));
    triggers.push_back(new TriggerNode("quest update complete", NextAction::array(0, new NextAction("quest update complete", relevance), nullptr)));
    triggers.push_back(new TriggerNode("questgiver quest details", NextAction::array(0, new NextAction("turn in query quest", relevance), nullptr)));
    triggers.push_back(new TriggerNode("item push result", NextAction::array(0, new NextAction("quest item push result", relevance), nullptr)));
    triggers.push_back(new TriggerNode("lfg proposal", NextAction::array(0, new NextAction("lfg accept", relevance), nullptr)));
    triggers.push_back(new TriggerNode("lfg proposal active", NextAction::array(0, new NextAction("lfg accept", relevance), nullptr)));
    triggers.push_back(new TriggerNode("lfg teleport", NextAction::array(0, new NextAction("lfg teleport", relevance), nullptr)));
    triggers.push_back(new TriggerNode("loot response", NextAction::array(0, new NextAction("store loot", relevance), nullptr)));
    triggers.push_back(new TriggerNode("master loot roll", NextAction::array(0, new NextAction("master loot roll", relevance), nullptr)));
}

WorldPacketHandlerStrategy::WorldPacketHandlerStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI)
{
    supported.push_back("uninvite");
    supported.push_back("lfg role check");
    supported.push_back("lfg teleport");
    supported.push_back("bg status");
    supported.push_back("check mount state");
    supported.push_back("confirm quest");
    supported.push_back("quest update add kill");
    supported.push_back("quest update failed");
    supported.push_back("quest update failed timer");
    supported.push_back("quest update complete");
    supported.push_back("turn in query quest");
    supported.push_back("quest item push result");
    supported.push_back("loot response");
    supported.push_back("master loot roll");
}

void ReadyCheckStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    PassTroughStrategy::InitTriggers(triggers);
}
