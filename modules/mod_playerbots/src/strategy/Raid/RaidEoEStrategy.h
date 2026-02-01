#ifndef _PLAYERBOT_RAIDEOESTRATEGY_H
#define _PLAYERBOT_RAIDEOESTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RaidEoEStrategy : public Strategy
{
public:
    RaidEoEStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "wotlk-eoe"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
