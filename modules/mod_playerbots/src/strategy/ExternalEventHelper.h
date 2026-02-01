

#ifndef _PLAYERBOT_EXTERNALEVENTHELPER_H
#define _PLAYERBOT_EXTERNALEVENTHELPER_H

#include <map>

#include "Common.h"

class AiObjectContext;
class Player;
class WorldPacket;

class ExternalEventHelper
{
public:
    ExternalEventHelper(AiObjectContext* aiObjectContext) : aiObjectContext(aiObjectContext) {}

    bool ParseChatCommand(std::string const command, Player* owner = nullptr);
    void HandlePacket(std::map<uint16, std::string>& handlers, WorldPacket const& packet, Player* owner = nullptr);
    bool HandleCommand(std::string const name, std::string const param, Player* owner = nullptr);

private:
    AiObjectContext* aiObjectContext;
};

#endif
