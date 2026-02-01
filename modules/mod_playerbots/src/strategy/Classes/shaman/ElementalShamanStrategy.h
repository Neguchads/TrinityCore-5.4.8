#ifndef _PLAYERBOT_ELEMENTALSHAMANSTRATEGY_H
#define _PLAYERBOT_ELEMENTALSHAMANSTRATEGY_H

#include "CasterShamanStrategy.h"

class PlayerbotAI;

class ElementalShamanStrategy : public CasterShamanStrategy
{
public:
    ElementalShamanStrategy(PlayerbotAI* botAI);

    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "ele"; }
};

#endif
