#ifndef _PLAYERBOT_REACTIONSTRATEGY_H
#define _PLAYERBOT_REACTIONSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class ReactionStrategy : public Strategy
{
public:
    ReactionStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "reaction"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
