#ifndef _PLAYERBOT_UTGARDEPINNACLESTRATEGY_H
#define _PLAYERBOT_UTGARDEPINNACLESTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class UtgardePinnacleStrategy : public Strategy
{
public:
    UtgardePinnacleStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "utgarde pinnacle"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
