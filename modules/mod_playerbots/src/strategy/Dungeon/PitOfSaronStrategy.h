#ifndef _PLAYERBOT_PITOFSARONSTRATEGY_H
#define _PLAYERBOT_PITOFSARONSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class PitOfSaronStrategy : public Strategy
{
public:
    PitOfSaronStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "pit of saron"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
