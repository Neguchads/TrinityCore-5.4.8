#ifndef _PLAYERBOT_WANDERSTRATEGY_H
#define _PLAYERBOT_WANDERSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class WanderStrategy : public Strategy
{
public:
    WanderStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "wander"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
