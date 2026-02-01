#ifndef _PLAYERBOT_CULLINGOFSTRATHOLMESTRATEGY_H
#define _PLAYERBOT_CULLINGOFSTRATHOLMESTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class CullingOfStratholmeStrategy : public Strategy
{
public:
    CullingOfStratholmeStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "culling of stratholme"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
