

#ifndef _PLAYERBOT_HEALPALADINSTRATEGY_H
#define _PLAYERBOT_HEALPALADINSTRATEGY_H

#include "GenericPaladinStrategy.h"

class PlayerbotAI;

class HealPaladinStrategy : public GenericPaladinStrategy
{
public:
    HealPaladinStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "heal"; }
    NextAction** getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_HEAL | STRATEGY_TYPE_RANGED; }
};

#endif
