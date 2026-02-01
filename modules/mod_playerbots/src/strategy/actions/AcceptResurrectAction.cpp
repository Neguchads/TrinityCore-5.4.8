#include "AcceptResurrectAction.h"

#include "Event.h"
#include "Opcodes.h"
#include "Player.h"
#include "Playerbots.h"
#include "WorldPacket.h"

namespace
{
    ObjectGuid ExtractResurrectorGuid(WorldPacket& packet)
    {
        packet.rpos(0);

        uint32 nameLenField = 0;
        uint32 spellId = 0;
        uint32 unk = 0;

        packet >> nameLenField;
        packet >> spellId;
        packet >> unk;

        ObjectGuid guid;
        guid[3] = packet.ReadBit();
        packet.ReadBit(); // caster is creature flag
        packet.ReadBit(); // unk
        guid[1] = packet.ReadBit();
        guid[5] = packet.ReadBit();
        guid[2] = packet.ReadBit();
        guid[6] = packet.ReadBit();
        guid[0] = packet.ReadBit();
        guid[4] = packet.ReadBit();
        guid[7] = packet.ReadBit();
        uint32 nameLen = packet.ReadBits(6);

        packet.FlushBits();

        packet.ReadByteSeq(guid[7]);
        packet.ReadByteSeq(guid[3]);
        packet.ReadByteSeq(guid[5]);

        if (nameLen)
            packet.ReadString(nameLen);

        packet.ReadByteSeq(guid[2]);
        packet.ReadByteSeq(guid[4]);
        packet.ReadByteSeq(guid[1]);
        packet.ReadByteSeq(guid[6]);
        packet.ReadByteSeq(guid[0]);

        return guid;
    }
}

bool AcceptResurrectAction::Execute(Event event)
{
    if (!bot || bot->IsAlive() || !bot->IsRessurectRequested())
        return false;

    if (event.getPacket().empty())
        return false;

    WorldPacket request(event.getPacket());
    ObjectGuid resurrectorGuid = ExtractResurrectorGuid(request);

    if (!bot->IsRessurectRequestedBy(resurrectorGuid))
        return false;

    WorldPacket response(CMSG_RESURRECT_RESPONSE, 9);
    response << uint32(0); // accept

    response.WriteBit(resurrectorGuid[3]);
    response.WriteBit(resurrectorGuid[0]);
    response.WriteBit(resurrectorGuid[6]);
    response.WriteBit(resurrectorGuid[4]);
    response.WriteBit(resurrectorGuid[5]);
    response.WriteBit(resurrectorGuid[2]);
    response.WriteBit(resurrectorGuid[1]);
    response.WriteBit(resurrectorGuid[7]);

    response.WriteByteSeq(resurrectorGuid[7]);
    response.WriteByteSeq(resurrectorGuid[0]);
    response.WriteByteSeq(resurrectorGuid[1]);
    response.WriteByteSeq(resurrectorGuid[3]);
    response.WriteByteSeq(resurrectorGuid[4]);
    response.WriteByteSeq(resurrectorGuid[6]);
    response.WriteByteSeq(resurrectorGuid[2]);
    response.WriteByteSeq(resurrectorGuid[5]);

    bot->GetSession()->HandleResurrectResponseOpcode(response);
    return true;
}
