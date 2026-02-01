

#ifndef _PLAYERBOT_UNHOLYDKSTRATEGY_H
#define _PLAYERBOT_UNHOLYDKSTRATEGY_H

#include "GenericDKStrategy.h"

class PlayerbotAI;

class UnholyDKStrategy : public GenericDKStrategy
{
public:
    UnholyDKStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "unholy"; }
    NextAction** getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

class UnholyDKAoeStrategy : public CombatStrategy
{
public:
    UnholyDKAoeStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "unholy aoe"; }
};

#endif
