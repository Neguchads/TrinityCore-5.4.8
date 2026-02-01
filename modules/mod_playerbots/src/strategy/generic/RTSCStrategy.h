#ifndef _PLAYERBOT_RTSCSTRATEGY_H
#define _PLAYERBOT_RTSCSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RTSCStrategy : public Strategy
{
public:
    RTSCStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "RTSC"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RTSCSJumptrategy : public Strategy
{
public:
    RTSCSJumptrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "rtsc jump"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
