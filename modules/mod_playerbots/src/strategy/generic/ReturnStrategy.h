#ifndef _PLAYERBOT_RETURNSTRATEGY_H
#define _PLAYERBOT_RETURNSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class ReturnStrategy : public Strategy
{
public:
    ReturnStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "return"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
