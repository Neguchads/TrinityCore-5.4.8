

#ifndef _PLAYERBOT_DEADSTRATEGY_H
#define _PLAYERBOT_DEADSTRATEGY_H

#include "PassTroughStrategy.h"

class PlayerbotAI;

class DeadStrategy : public PassTroughStrategy
{
public:
    DeadStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "dead"; }
};

#endif
