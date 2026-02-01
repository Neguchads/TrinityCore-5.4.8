#ifndef _PLAYERBOT_COMBATROGUESTRATEGY_H
#define _PLAYERBOT_COMBATROGUESTRATEGY_H

#include "DpsRogueStrategy.h"

class PlayerbotAI;

class CombatRogueStrategy : public DpsRogueStrategy
{
public:
    CombatRogueStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "combat"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
