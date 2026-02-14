

#include "ExternalEventHelper.h"

//#include "ChatHelper.h"
#include "ChatHelper.h"
#include "Playerbots.h"
#include "Trigger.h"

bool ExternalEventHelper::ParseChatCommand(std::string const command, Player* owner)
{
    std::string filtered = command;
    std::string linkCommand = ChatHelper::parseValue("command", filtered);
    if (!linkCommand.empty())
        filtered = linkCommand;

    if (HandleCommand(filtered, "", owner))
        return true;

    size_t i = std::string::npos;
    while (true)
    {
        size_t found = filtered.rfind(" ", i);
        if (found == std::string::npos || !found)
            break;

        std::string const name = filtered.substr(0, found);
        std::string const param = filtered.substr(found + 1);

        i = found - 1;

        if (HandleCommand(name, param, owner))
            return true;
    }

    //if (!ChatHelper::parseable(command))
        //return false;

    HandleCommand("c", filtered, owner);
    HandleCommand("t", filtered, owner);

    return true;
}

void ExternalEventHelper::HandlePacket(std::map<uint16, std::string>& handlers, WorldPacket const& packet,
                                       Player* owner)
{
    uint16 opcode = packet.GetOpcode();
    std::string const name = handlers[opcode];
    if (name.empty())
        return;

    Trigger* trigger = aiObjectContext->GetTrigger(name);
    if (!trigger)
    {
        TC_LOG_ERROR("playerbots", "No trigger defined for packet: %s", name.c_str());
        return;
    }

    WorldPacket p(packet);
    trigger->ExternalEvent(p, owner);
}

bool ExternalEventHelper::HandleCommand(std::string const name, std::string const param, Player* owner)
{
    Trigger* trigger = aiObjectContext->GetTrigger(name);
    if (!trigger)
        return false;

    trigger->ExternalEvent(param, owner);

    return true;
}
