#ifndef _PLAYERBOT_VIOLETHOLDSTRATEGY_H
#define _PLAYERBOT_VIOLETHOLDSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class VioletHoldStrategy : public Strategy
{
public:
    VioletHoldStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "violet hold"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
