#ifndef _PLAYERBOT_RAIDGRUULSLAIRSTRATEGY_H
#define _PLAYERBOT_RAIDGRUULSLAIRSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidGruulsLairStrategy : public Strategy
{
public:
    RaidGruulsLairStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "gruulslair"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
