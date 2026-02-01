#ifndef _PLAYERBOT_RAIDAQ20STRATEGY_H
#define _PLAYERBOT_RAIDAQ20STRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidAq20Strategy : public Strategy
{
public:
    RaidAq20Strategy(PlayerbotAI* botAI);

    std::string const getName() override { return "aq20"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
