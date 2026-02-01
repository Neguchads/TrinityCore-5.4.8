#ifndef _PLAYERBOT_KARAZHANDUNGEONSTRATEGIES_H
#define _PLAYERBOT_KARAZHANDUNGEONSTRATEGIES_H

#include "Strategy.h"

class PlayerbotAI;

class KarazhanDungeonStrategy : public Strategy
{
public:
    KarazhanDungeonStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "karazhan"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class NetherspiteFightStrategy : public Strategy
{
public:
    NetherspiteFightStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "netherspite"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class PrinceMalchezaarFightStrategy : public Strategy
{
public:
    PrinceMalchezaarFightStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "prince malchezaar"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
