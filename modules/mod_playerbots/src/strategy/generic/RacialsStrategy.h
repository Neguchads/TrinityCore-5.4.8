#ifndef _PLAYERBOT_RACIALSSTRATEGY_H
#define _PLAYERBOT_RACIALSSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RacialsStrategy : public Strategy
{
public:
    RacialsStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "racials"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
