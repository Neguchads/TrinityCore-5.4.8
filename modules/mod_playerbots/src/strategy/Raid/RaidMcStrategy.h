#ifndef _PLAYERBOT_RAIDMCSTRATEGY_H
#define _PLAYERBOT_RAIDMCSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidMcStrategy : public Strategy
{
public:
    RaidMcStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "moltencore"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
