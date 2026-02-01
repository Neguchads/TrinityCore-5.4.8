#ifndef _PLAYERBOT_AZJOLNERUBSTRATEGY_H
#define _PLAYERBOT_AZJOLNERUBSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class AzjolNerubStrategy : public Strategy
{
public:
    AzjolNerubStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "azjol'nerub"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
