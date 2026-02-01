#ifndef _PLAYERBOT_DISCIPLINEPRIESTSTRATEGY_H
#define _PLAYERBOT_DISCIPLINEPRIESTSTRATEGY_H

#include "HealPriestStrategy.h"

class PlayerbotAI;

class DisciplinePriestStrategy : public HealPriestStrategy
{
public:
    DisciplinePriestStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "discipline"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
