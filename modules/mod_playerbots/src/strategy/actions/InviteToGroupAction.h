#ifndef _PLAYERBOT_INVITETOGROUPACTION_H
#define _PLAYERBOT_INVITETOGROUPACTION_H

#include <vector>

#include "Action.h"
#include "Player.h"

class Player;
class PlayerbotAI;

class InviteToGroupAction : public Action
{
public:
    InviteToGroupAction(PlayerbotAI* botAI, std::string const name = "invite") : Action(botAI, name) {}

    bool Execute(Event event) override;

protected:
    bool Invite(Player* inviter, Player* player);
};

class JoinGroupAction : public InviteToGroupAction
{
public:
    JoinGroupAction(PlayerbotAI* botAI, std::string const name = "join") : InviteToGroupAction(botAI, name) {}

    bool Execute(Event event) override;
};

class InviteNearbyToGroupAction : public InviteToGroupAction
{
public:
    InviteNearbyToGroupAction(PlayerbotAI* botAI, std::string const name = "invite nearby") : InviteToGroupAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class FindGuildMembers
{
public:
    void operator()(Player* player) { data.push_back(player); }
    std::vector<Player*> const GetResult() const { return data; }

private:
    std::vector<Player*> data;
};

class InviteGuildToGroupAction : public InviteNearbyToGroupAction
{
public:
    InviteGuildToGroupAction(PlayerbotAI* botAI, std::string const name = "invite guild")
        : InviteNearbyToGroupAction(botAI, name)
    {
    }

    bool Execute(Event event) override;
    bool isUseful() override { return bot->GetGuildId() && InviteNearbyToGroupAction::isUseful(); }

private:
    std::vector<Player*> GetGuildMembers();
};

class LfgAction : public InviteToGroupAction
{
public:
    LfgAction(PlayerbotAI* botAI, std::string const name = "lfg") : InviteToGroupAction(botAI, name) {}

    bool Execute(Event event) override;
};

#endif
