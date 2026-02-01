#ifndef _PLAYERBOT_SAYSTRATEGY_H
#define _PLAYERBOT_SAYSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class SayStrategy : public Strategy
{
public:
    SayStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "say"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
