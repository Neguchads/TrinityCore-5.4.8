#ifndef _PLAYERBOT_DKREACTIONSTRATEGY_H
#define _PLAYERBOT_DKREACTIONSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class DKReactionStrategy : public Strategy
{
public:
    DKReactionStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "react"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
