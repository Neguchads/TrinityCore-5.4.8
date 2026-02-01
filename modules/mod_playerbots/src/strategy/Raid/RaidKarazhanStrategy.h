#ifndef _PLAYERBOT_RAIDKARAZHANSTRATEGY_H
#define _PLAYERBOT_RAIDKARAZHANSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidKarazhanStrategy : public Strategy
{
public:
    RaidKarazhanStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "karazhan"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
