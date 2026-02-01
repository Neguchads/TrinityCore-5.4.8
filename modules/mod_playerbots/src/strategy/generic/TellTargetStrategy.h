#ifndef _PLAYERBOT_TELLTARGETSTRATEGY_H
#define _PLAYERBOT_TELLTARGETSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class TellTargetStrategy : public Strategy
{
public:
    TellTargetStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "TellTarget"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
