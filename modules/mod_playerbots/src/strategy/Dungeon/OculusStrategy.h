#ifndef _PLAYERBOT_OCULUSSTRATEGY_H
#define _PLAYERBOT_OCULUSSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class OculusStrategy : public Strategy
{
public:
    OculusStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "oculus"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
