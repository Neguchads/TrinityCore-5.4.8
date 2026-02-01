#ifndef _PLAYERBOT_GRINDINGSTRATEGY_H
#define _PLAYERBOT_GRINDINGSTRATEGY_H

#include "NonCombatStrategy.h"

class PlayerbotAI;

class GrindingStrategy : public Strategy
{
public:
    GrindingStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "grind"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class MoveRandomStrategy : public NonCombatStrategy
{
public:
    MoveRandomStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI) {}

    std::string const getName() override { return "move random"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
