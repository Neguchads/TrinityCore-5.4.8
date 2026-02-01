#ifndef _PLAYERBOT_INVITETOGROUPACTION_H
#define _PLAYERBOT_INVITETOGROUPACTION_H

#include "Action.h"

class Player;
class PlayerbotAI;

class InviteToGroupAction : public Action
{
public:
    InviteToGroupAction(PlayerbotAI* botAI, std::string const name = "invite") : Action(botAI, name) {}

protected:
    bool Invite(Player* inviter, Player* player);
};

class LfgAction : public InviteToGroupAction
{
public:
    LfgAction(PlayerbotAI* botAI, std::string const name = "lfg") : InviteToGroupAction(botAI, name) {}

    bool Execute(Event event) override;
};

#endif
