#ifndef _PLAYERBOT_GUNDRAKSTRATEGY_H
#define _PLAYERBOT_GUNDRAKSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class GundrakStrategy : public Strategy
{
public:
    GundrakStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "gundrak"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
