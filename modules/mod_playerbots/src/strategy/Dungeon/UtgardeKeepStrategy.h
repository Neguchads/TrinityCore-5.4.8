#ifndef _PLAYERBOT_UTGARDEKEEPSTRATEGY_H
#define _PLAYERBOT_UTGARDEKEEPSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class UtgardeKeepStrategy : public Strategy
{
public:
    UtgardeKeepStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "utgarde keep"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
