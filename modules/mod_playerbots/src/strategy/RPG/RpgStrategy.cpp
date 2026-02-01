#include "RpgStrategy.h"

#include "Playerbots.h"

RpgStrategy::RpgStrategy(PlayerbotAI* botAI) : NewRpgStrategy(botAI) {}

void RpgQuestStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("new rpg status update", 1.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("near npc status", NextAction::array(0, new NextAction("new rpg move npc", 1.0f), nullptr)));
}

void RpgVendorStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("new rpg status update", 1.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("near npc status", NextAction::array(0, new NextAction("new rpg move npc", 1.0f), nullptr)));
}

void RpgExploreStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("new rpg status update", 1.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("near random status", NextAction::array(0, new NextAction("new rpg move random", 1.0f), nullptr)));
}

void RpgMaintenanceStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("new rpg status update", 1.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("go innkeeper status", NextAction::array(0, new NextAction("new rpg go innkeeper", 1.0f), nullptr)));
}

void RpgGuildStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("new rpg status update", 1.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("near npc status", NextAction::array(0, new NextAction("new rpg move npc", 1.0f), nullptr)));
}

void RpgBgStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("new rpg status update", 1.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("go grind status", NextAction::array(0, new NextAction("new rpg go grind", 1.0f), nullptr)));
}

void RpgPlayerStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("new rpg status update", 1.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("near npc status", NextAction::array(0, new NextAction("new rpg move npc", 1.0f), nullptr)));
}

void RpgCraftStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("new rpg status update", 1.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("near npc status", NextAction::array(0, new NextAction("new rpg move npc", 1.0f), nullptr)));
}

void RpgJumpStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("new rpg move random", 1.0f), nullptr)));
}
