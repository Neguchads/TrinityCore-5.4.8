#ifndef _PLAYERBOT_RETRIBUTIONPALADINSTRATEGY_H
#define _PLAYERBOT_RETRIBUTIONPALADINSTRATEGY_H

#include "DpsPaladinStrategy.h"

class PlayerbotAI;

class RetributionPaladinStrategy : public DpsPaladinStrategy
{
public:
    RetributionPaladinStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "retribution"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
