#ifndef _PLAYERBOT_BALANCEDRUIDSTRATEGY_H
#define _PLAYERBOT_BALANCEDRUIDSTRATEGY_H

#include "CasterDruidStrategy.h"

class PlayerbotAI;

class BalanceDruidStrategy : public CasterDruidStrategy
{
public:
    BalanceDruidStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "balance"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
