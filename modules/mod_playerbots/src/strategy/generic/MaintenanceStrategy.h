#ifndef _PLAYERBOT_MAINTENANCESTRATEGY_H
#define _PLAYERBOT_MAINTENANCESTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class MaintenanceStrategy : public Strategy
{
public:
    MaintenanceStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "maintenance"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
