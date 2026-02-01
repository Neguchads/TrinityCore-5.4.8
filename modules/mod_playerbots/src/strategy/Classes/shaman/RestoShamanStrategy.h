#ifndef _PLAYERBOT_RESTOSHAMANSTRATEGY_H
#define _PLAYERBOT_RESTOSHAMANSTRATEGY_H

#include "HealShamanStrategy.h"

class PlayerbotAI;

class RestoShamanStrategy : public HealShamanStrategy
{
public:
    RestoShamanStrategy(PlayerbotAI* botAI);

    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "resto"; }
};

#endif
