#ifndef _PLAYERBOT_MOLTENCOREDUNGEONSTRATEGIES_H
#define _PLAYERBOT_MOLTENCOREDUNGEONSTRATEGIES_H

#include "Strategy.h"

class PlayerbotAI;

class MoltenCoreDungeonStrategy : public Strategy
{
public:
    MoltenCoreDungeonStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "molten core"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class MagmadarFightStrategy : public Strategy
{
public:
    MagmadarFightStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "magmadar"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
