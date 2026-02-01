#include "AttackEnemyPlayersStrategy.h"

#include "Playerbots.h"

void AttackEnemyPlayersStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("enemy player near", NextAction::array(0, new NextAction("attack enemy player", 55.0f), nullptr)));
}
