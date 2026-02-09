#include "NonCombatStrategy.h"

#include "Playerbots.h"

void NonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("timer", NextAction::array(0, new NextAction("check mount state", 1.0f), nullptr)));
}

void CollisionStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("collision", NextAction::array(0, new NextAction("move out of collision", 2.0f), nullptr)));
}

void MountStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void WorldBuffStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void NoWarStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void GlyphStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void FishStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}

void MasterFishingStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
}
