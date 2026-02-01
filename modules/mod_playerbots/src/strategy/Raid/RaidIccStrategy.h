#ifndef _PLAYERBOT_RAIDICCSTRATEGY_H
#define _PLAYERBOT_RAIDICCSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidIccStrategy : public Strategy
{
public:
    RaidIccStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "icc"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
