#ifndef _PLAYERBOT_HALLSOFLIGHTNINGSTRATEGY_H
#define _PLAYERBOT_HALLSOFLIGHTNINGSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class HallsOfLightningStrategy : public Strategy
{
public:
    HallsOfLightningStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "halls of lightning"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
