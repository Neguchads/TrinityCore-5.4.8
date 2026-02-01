#ifndef _PLAYERBOT_FOLLOWMASTERSTRATEGY_H
#define _PLAYERBOT_FOLLOWMASTERSTRATEGY_H

#include "NonCombatStrategy.h"

class PlayerbotAI;

class FollowMasterStrategy : public NonCombatStrategy
{
public:
    FollowMasterStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI) {}

    std::string const getName() override { return "follow"; }
    NextAction** getDefaultActions() override;
};

class FreeStrategy : public Strategy
{
public:
    FreeStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "free"; }
};

class FollowJumpStrategy : public Strategy
{
public:
    FollowJumpStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "follow jump"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
