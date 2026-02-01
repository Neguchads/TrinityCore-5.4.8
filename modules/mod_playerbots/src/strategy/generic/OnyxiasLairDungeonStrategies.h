#ifndef _PLAYERBOT_ONYXIASLAIRDUNGEONSTRATEGIES_H
#define _PLAYERBOT_ONYXIASLAIRDUNGEONSTRATEGIES_H

#include "Strategy.h"

class PlayerbotAI;

class OnyxiasLairDungeonStrategy : public Strategy
{
public:
    OnyxiasLairDungeonStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "onyxia's lair"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class OnyxiaFightStrategy : public Strategy
{
public:
    OnyxiaFightStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "onyxia"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
