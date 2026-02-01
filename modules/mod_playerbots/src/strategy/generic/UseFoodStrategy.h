#ifndef _PLAYERBOT_USEFOODSTRATEGY_H
#define _PLAYERBOT_USEFOODSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class UseFoodStrategy : public Strategy
{
public:
    UseFoodStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "food"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
