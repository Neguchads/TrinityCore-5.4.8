#ifndef _PLAYERBOT_NAXXRAMASDUNGEONSTRATEGIES_H
#define _PLAYERBOT_NAXXRAMASDUNGEONSTRATEGIES_H

#include "Strategy.h"

class PlayerbotAI;

class NaxxramasDungeonStrategy : public Strategy
{
public:
    NaxxramasDungeonStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "naxxramas"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class FourHorsemanFightStrategy : public Strategy
{
public:
    FourHorsemanFightStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "four horseman"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
