

#ifndef _PLAYERBOT_TANKWARRIORSTRATEGY_H
#define _PLAYERBOT_TANKWARRIORSTRATEGY_H

#include "GenericWarriorStrategy.h"
#include "WarriorTriggers.h"

class PlayerbotAI;

class TankWarriorStrategy : public GenericWarriorStrategy
{
public:
    TankWarriorStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "tank"; }
    NextAction** getDefaultActions() override;
    uint32 GetType() const override { return STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
};

#endif
