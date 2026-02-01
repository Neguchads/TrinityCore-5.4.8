#ifndef _PLAYERBOT_DUNGEONSTRATEGY_H
#define _PLAYERBOT_DUNGEONSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class DungeonStrategy : public Strategy
{
public:
    DungeonStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "dungeon"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
