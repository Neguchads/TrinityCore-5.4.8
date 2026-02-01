#ifndef _PLAYERBOT_ATTACKENEMYPLAYERSSTRATEGY_H
#define _PLAYERBOT_ATTACKENEMYPLAYERSSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class AttackEnemyPlayersStrategy : public Strategy
{
public:
    AttackEnemyPlayersStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "pvp"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
