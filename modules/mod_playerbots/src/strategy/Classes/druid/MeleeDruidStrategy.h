

#ifndef _PLAYERBOT_MELEEDRUIDSTRATEGY_H
#define _PLAYERBOT_MELEEDRUIDSTRATEGY_H

#include "CombatStrategy.h"

class MeleeDruidStrategy : public CombatStrategy
{
public:
    MeleeDruidStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "melee"; }
    NextAction** getDefaultActions() override;
};

#endif
