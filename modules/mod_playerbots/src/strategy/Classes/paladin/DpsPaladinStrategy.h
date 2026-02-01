

#ifndef _PLAYERBOT_DPSPALADINSTRATEGY_H
#define _PLAYERBOT_DPSPALADINSTRATEGY_H

#include "GenericPaladinStrategy.h"

class PlayerbotAI;

class DpsPaladinStrategy : public GenericPaladinStrategy
{
public:
    DpsPaladinStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "dps"; }
    NextAction** getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
};

#endif
