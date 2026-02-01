#ifndef _PLAYERBOT_HALLSOFSTONESTRATEGY_H
#define _PLAYERBOT_HALLSOFSTONESTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class HallsOfStoneStrategy : public Strategy
{
public:
    HallsOfStoneStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "halls of stone"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
