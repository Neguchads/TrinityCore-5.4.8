#ifndef _PLAYERBOT_GUILDBANKACTIONS_H
#define _PLAYERBOT_GUILDBANKACTIONS_H

#include "Action.h"

// Chat command: "gb <itemId|all>"
// Deposits items from bags into the first guild bank tab.
class GuildBankCommandAction : public Action
{
public:
    GuildBankCommandAction(PlayerbotAI* botAI, std::string const& name = "gb") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

#endif
