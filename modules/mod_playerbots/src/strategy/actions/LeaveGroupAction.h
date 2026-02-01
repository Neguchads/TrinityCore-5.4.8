

#ifndef _PLAYERBOT_LEAVEGROUPACTION_H
#define _PLAYERBOT_LEAVEGROUPACTION_H

#include "Action.h"

class Player;
class PlayerbotAI;

class LeaveGroupAction : public Action
{
public:
    LeaveGroupAction(PlayerbotAI* botAI, std::string const name = "leave") : Action(botAI, name) {}

    bool Execute(Event event) override;

    virtual bool Leave(Player* player);
};

class UninviteAction : public LeaveGroupAction
{
public:
    UninviteAction(PlayerbotAI* botAI) : LeaveGroupAction(botAI, "uninvite") {}

    bool Execute(Event event) override;
};


#endif
