#ifndef _PLAYERBOT_GUILDMANAGEMENTACTIONS_H
#define _PLAYERBOT_GUILDMANAGEMENTACTIONS_H

#include "Action.h"

class PlayerbotAI;

// Minimal CMaNGOS-parity guild management actions:
// - invite nearby players/bots to the bot's guild (random bots)
// - leave guild (safety/overflow)
class GuildManageNearbyAction : public Action
{
public:
    GuildManageNearbyAction(PlayerbotAI* botAI) : Action(botAI, "guild manage nearby") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class GuildInviteAction : public Action
{
public:
    GuildInviteAction(PlayerbotAI* botAI) : Action(botAI, "guild invite") {}
    bool Execute(Event event) override;
};

class GuildJoinAction : public Action
{
public:
    GuildJoinAction(PlayerbotAI* botAI) : Action(botAI, "guild join") {}
    bool Execute(Event event) override;
};

class GuildPromoteAction : public Action
{
public:
    GuildPromoteAction(PlayerbotAI* botAI) : Action(botAI, "guild promote") {}
    bool Execute(Event event) override;
};

class GuildDemoteAction : public Action
{
public:
    GuildDemoteAction(PlayerbotAI* botAI) : Action(botAI, "guild demote") {}
    bool Execute(Event event) override;
};

class GuildRemoveAction : public Action
{
public:
    GuildRemoveAction(PlayerbotAI* botAI) : Action(botAI, "guild remove") {}
    bool Execute(Event event) override;
};

class GuildLeaderAction : public Action
{
public:
    GuildLeaderAction(PlayerbotAI* botAI) : Action(botAI, "guild leader") {}
    bool Execute(Event event) override;
};

class GuildLeaveAction : public Action
{
public:
    GuildLeaveAction(PlayerbotAI* botAI) : Action(botAI, "guild leave") {}
    bool Execute(Event event) override;
};

#endif
