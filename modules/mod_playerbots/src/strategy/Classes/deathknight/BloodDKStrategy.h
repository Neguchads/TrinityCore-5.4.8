

#ifndef _PLAYERBOT_BLOODDKSTRATEGY_H
#define _PLAYERBOT_BLOODDKSTRATEGY_H

#include "GenericDKStrategy.h"

class PlayerbotAI;

class BloodDKStrategy : public GenericDKStrategy
{
public:
    BloodDKStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "blood"; }
    NextAction** getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
};

#endif
