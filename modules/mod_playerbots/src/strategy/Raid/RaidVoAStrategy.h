#ifndef _PLAYERBOT_RAIDVOASTRATEGY_H
#define _PLAYERBOT_RAIDVOASTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidVoAStrategy : public Strategy
{
public:
    RaidVoAStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "voa"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
