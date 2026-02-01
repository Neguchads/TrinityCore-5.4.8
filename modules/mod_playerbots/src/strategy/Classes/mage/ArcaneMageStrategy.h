

#ifndef _PLAYERBOT_ARCANEMAGESTRATEGY_H
#define _PLAYERBOT_ARCANEMAGESTRATEGY_H

#include "GenericMageStrategy.h"

class PlayerbotAI;

class ArcaneMageStrategy : public GenericMageStrategy
{
public:
    ArcaneMageStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "arcane"; }
    NextAction** getDefaultActions() override;
};

class ArcaneMageAoeStrategy : public CombatStrategy
{
public:
    ArcaneMageAoeStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}

public:
    virtual void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "arcane aoe"; }
};
#endif
