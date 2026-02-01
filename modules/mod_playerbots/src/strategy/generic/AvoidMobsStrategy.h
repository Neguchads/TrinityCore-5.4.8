#ifndef _PLAYERBOT_AVOIDMOBSSTRATEGY_H
#define _PLAYERBOT_AVOIDMOBSSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class AvoidMobsStrategy : public Strategy
{
public:
    AvoidMobsStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "avoid mobs"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
