

#include "Event.h"

#include "Playerbots.h"

Event::Event(std::string const source, ObjectGuid object, Player* owner) : source(source), owner(owner)
{
    packet << object;
}

ObjectGuid Event::getObject()
{
    if (packet.empty())
        return ObjectGuid::Empty;

    WorldPacket p(packet);
    p.rpos(0);

    ObjectGuid guid;
    p >> guid;

    return guid;
}
