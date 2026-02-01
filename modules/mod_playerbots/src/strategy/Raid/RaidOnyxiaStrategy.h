#ifndef _PLAYERBOT_RAIDONYXIASTRATEGY_H
#define _PLAYERBOT_RAIDONYXIASTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidOnyxiaStrategy : public Strategy
{
public:
    RaidOnyxiaStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "onyxia"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
