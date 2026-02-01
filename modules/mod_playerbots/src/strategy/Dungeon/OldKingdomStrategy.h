#ifndef _PLAYERBOT_OLDKINGDOMSTRATEGY_H
#define _PLAYERBOT_OLDKINGDOMSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class OldKingdomStrategy : public Strategy
{
public:
    OldKingdomStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "old kingdom"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
