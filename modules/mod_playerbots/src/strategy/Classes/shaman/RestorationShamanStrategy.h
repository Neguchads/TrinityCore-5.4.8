#ifndef _PLAYERBOT_RESTORATIONSHAMANSTRATEGY_H
#define _PLAYERBOT_RESTORATIONSHAMANSTRATEGY_H

#include "HealShamanStrategy.h"

class PlayerbotAI;

class RestorationShamanStrategy : public HealShamanStrategy
{
public:
    RestorationShamanStrategy(PlayerbotAI* botAI);

    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "restoration"; }
};

#endif
