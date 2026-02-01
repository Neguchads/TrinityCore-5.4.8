#ifndef _PLAYERBOT_KITESTRATEGY_H
#define _PLAYERBOT_KITESTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class KiteStrategy : public Strategy
{
public:
    KiteStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "kite"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
