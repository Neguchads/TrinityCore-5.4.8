#ifndef _PLAYERBOT_RAIDOSSTRATEGY_H
#define _PLAYERBOT_RAIDOSSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidOsStrategy : public Strategy
{
public:
    RaidOsStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "wotlk-os"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
