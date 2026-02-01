

#ifndef _PLAYERBOT_FIREMAGESTRATEGY_H
#define _PLAYERBOT_FIREMAGESTRATEGY_H

#include "GenericMageStrategy.h"

class PlayerbotAI;

class FireMageStrategy : public GenericMageStrategy
{
public:
    FireMageStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "fire"; }
    NextAction** getDefaultActions() override;
};

class FireMageAoeStrategy : public CombatStrategy
{
public:
    FireMageAoeStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "fire aoe"; }
};

#endif
