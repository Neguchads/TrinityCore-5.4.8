

#ifndef _PLAYERBOT_TOTEMSSHAMANSTRATEGY_H
#define _PLAYERBOT_TOTEMSSHAMANSTRATEGY_H

#include "GenericShamanStrategy.h"

class PlayerbotAI;

class TotemsShamanStrategy : public GenericShamanStrategy
{
public:
    TotemsShamanStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "totems"; }
};

#endif
