#ifndef _PLAYERBOT_FORGEOFSOULSSTRATEGY_H
#define _PLAYERBOT_FORGEOFSOULSSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class ForgeOfSoulsStrategy : public Strategy
{
public:
    ForgeOfSoulsStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "forge of souls"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
