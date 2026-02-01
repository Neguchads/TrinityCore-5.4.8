#ifndef _PLAYERBOT_USEPOTIONSSTRATEGY_H
#define _PLAYERBOT_USEPOTIONSSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class UsePotionsStrategy : public Strategy
{
public:
    UsePotionsStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "potions"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
