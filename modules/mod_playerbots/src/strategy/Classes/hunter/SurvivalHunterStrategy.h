#ifndef _PLAYERBOT_SURVIVALHUNTERSTRATEGY_H
#define _PLAYERBOT_SURVIVALHUNTERSTRATEGY_H

#include "DpsHunterStrategy.h"

class PlayerbotAI;

class SurvivalHunterStrategy : public DpsHunterStrategy
{
public:
    SurvivalHunterStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "surv"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
