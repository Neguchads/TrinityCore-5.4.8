#ifndef _PLAYERBOT_MARKRTISTRATEGY_H
#define _PLAYERBOT_MARKRTISTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class MarkRtiStrategy : public Strategy
{
public:
    MarkRtiStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "mark rti"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
