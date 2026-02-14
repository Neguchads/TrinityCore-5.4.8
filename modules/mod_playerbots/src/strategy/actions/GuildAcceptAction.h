#ifndef _PLAYERBOT_GUILDACCEPTACTION_H
#define _PLAYERBOT_GUILDACCEPTACTION_H

#include "Action.h"

// WorldPacket-driven action (SMSG_GUILD_INVITE).
class GuildAcceptAction : public Action
{
public:
    GuildAcceptAction(PlayerbotAI* botAI, std::string const& name = "guild accept") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

#endif

