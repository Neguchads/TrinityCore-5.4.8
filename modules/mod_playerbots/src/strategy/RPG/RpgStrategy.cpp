#include "RpgStrategy.h"

#include "Playerbots.h"
#include "RpgSubActions.h"

float RpgActionMultiplier::GetValue(Action* action)
{
    if (!action)
        return 1.0f;

    std::string const nextAction = AI_VALUE(std::string, "next rpg action");
    std::string const name = action->getName();

    if (!nextAction.empty() && dynamic_cast<RpgEnabled*>(action) && name != nextAction)
        return 0.0f;

    return 1.0f;
}

RpgStrategy::RpgStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

NextAction** RpgStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("rpg", 1.0f), nullptr);
}

void RpgStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("no rpg target", NextAction::array(0, new NextAction("choose rpg target", 5.0f), nullptr)));
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("move random", 1.10f), nullptr)));
    triggers.push_back(new TriggerNode("far from rpg target", NextAction::array(0, new NextAction("move to rpg target", 5.0f), nullptr)));

    // Sub actions
    triggers.push_back(new TriggerNode("rpg", NextAction::array(0, new NextAction("rpg stay", 1.101f), nullptr)));
    triggers.push_back(new TriggerNode("rpg", NextAction::array(0, new NextAction("rpg work", 1.101f), nullptr)));
    triggers.push_back(new TriggerNode("rpg", NextAction::array(0, new NextAction("rpg emote", 1.101f), nullptr)));
    triggers.push_back(new TriggerNode("has rpg target", NextAction::array(0, new NextAction("rpg cancel", 1.101f), nullptr)));
    triggers.push_back(new TriggerNode("rpg discover", NextAction::array(0, new NextAction("rpg discover", 1.210f), nullptr)));
    triggers.push_back(new TriggerNode("rpg start quest", NextAction::array(0, new NextAction("rpg start quest", 1.180f), nullptr)));
    triggers.push_back(new TriggerNode("rpg end quest", NextAction::array(0, new NextAction("rpg end quest", 1.190f), nullptr)));
    triggers.push_back(new TriggerNode("rpg buy", NextAction::array(0, new NextAction("rpg buy", 1.130f), nullptr)));
    triggers.push_back(new TriggerNode("rpg repair", NextAction::array(0, new NextAction("rpg repair", 1.195f), nullptr)));
    triggers.push_back(new TriggerNode("rpg heal", NextAction::array(0, new NextAction("rpg heal", 1.125f), nullptr)));
    triggers.push_back(new TriggerNode("rpg home bind", NextAction::array(0, new NextAction("rpg home bind", 1.160f), nullptr)));
    triggers.push_back(new TriggerNode("rpg buy petition", NextAction::array(0, new NextAction("rpg buy petition", 1.140f), nullptr)));
    triggers.push_back(new TriggerNode("rpg use", NextAction::array(0, new NextAction("rpg use", 1.102f), nullptr)));
}

void RpgStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new RpgActionMultiplier(botAI));
}

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
