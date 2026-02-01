#ifndef _PLAYERBOT_PULLSTRATEGY_H
#define _PLAYERBOT_PULLSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class PullStrategy : public Strategy
{
public:
    PullStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "pull"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class PossibleAddsStrategy : public Strategy
{
public:
    PossibleAddsStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "adds"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class PossibleAdsStrategy : public PossibleAddsStrategy
{
public:
    PossibleAdsStrategy(PlayerbotAI* botAI) : PossibleAddsStrategy(botAI) {}

    std::string const getName() override { return "ads"; }
};

class PullBackStrategy : public Strategy
{
public:
    PullBackStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "pull back"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
