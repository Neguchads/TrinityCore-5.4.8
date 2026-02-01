#ifndef _PLAYERBOT_ENHANCEMENTSHAMANSTRATEGY_H
#define _PLAYERBOT_ENHANCEMENTSHAMANSTRATEGY_H

#include "MeleeShamanStrategy.h"

class PlayerbotAI;

class EnhancementShamanStrategy : public MeleeShamanStrategy
{
public:
    EnhancementShamanStrategy(PlayerbotAI* botAI);

    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "enh"; }
};

#endif
