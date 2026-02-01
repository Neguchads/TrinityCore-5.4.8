#ifndef _PLAYERBOT_GUILDSTRATEGY_H
#define _PLAYERBOT_GUILDSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class GuildStrategy : public Strategy
{
public:
    GuildStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "guild"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
