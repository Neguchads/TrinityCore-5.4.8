#ifndef _PLAYERBOT_DRAKTHARONKEEPSTRATEGY_H
#define _PLAYERBOT_DRAKTHARONKEEPSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class DrakTharonKeepStrategy : public Strategy
{
public:
    DrakTharonKeepStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "drak'tharon keep"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
