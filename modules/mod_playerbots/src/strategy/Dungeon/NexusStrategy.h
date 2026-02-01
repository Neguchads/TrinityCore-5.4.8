#ifndef _PLAYERBOT_NEXUSSTRATEGY_H
#define _PLAYERBOT_NEXUSSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class NexusStrategy : public Strategy
{
public:
    NexusStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "nexus"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
