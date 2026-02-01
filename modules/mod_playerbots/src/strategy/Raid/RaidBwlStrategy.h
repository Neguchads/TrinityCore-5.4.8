#ifndef _PLAYERBOT_RAIDBWLSTRATEGY_H
#define _PLAYERBOT_RAIDBWLSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidBwlStrategy : public Strategy
{
public:
    RaidBwlStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "bwl"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
