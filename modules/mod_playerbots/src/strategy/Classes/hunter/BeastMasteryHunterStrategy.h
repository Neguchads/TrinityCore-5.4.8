#ifndef _PLAYERBOT_BEASTMASTERYHUNTERSTRATEGY_H
#define _PLAYERBOT_BEASTMASTERYHUNTERSTRATEGY_H

#include "DpsHunterStrategy.h"

class PlayerbotAI;

class BeastMasteryHunterStrategy : public DpsHunterStrategy
{
public:
    BeastMasteryHunterStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "bm"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
