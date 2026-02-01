#ifndef _PLAYERBOT_MAINTENANCEACTION_H
#define _PLAYERBOT_MAINTENANCEACTION_H

#include "Action.h"

class PlayerbotAI;

class MaintenanceAction : public Action
{
public:
    MaintenanceAction(PlayerbotAI* botAI, std::string const name = "maintenance")
        : Action(botAI, name)
    {
    }

    bool Execute(Event event) override;
};

#endif
