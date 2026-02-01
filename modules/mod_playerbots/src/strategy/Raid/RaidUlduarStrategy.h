#ifndef _PLAYERBOT_RAIDULDUARSTRATEGY_H
#define _PLAYERBOT_RAIDULDUARSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidUlduarStrategy : public Strategy
{
public:
    RaidUlduarStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "uld"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
