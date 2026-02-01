#include "AcceptDuelAction.h"

#include "Event.h"
#include "Opcodes.h"
#include "Playerbots.h"
#include "WorldPacket.h"

bool AcceptDuelAction::Execute(Event /*event*/)
{
    if (!bot || !bot->duel || !bot->duel->opponent)
        return false;

    ObjectGuid challengerGuid = bot->duel->opponent->GetGUID();
    bool fromMaster = botAI->HasRealPlayerMaster() && botAI->GetMaster() && botAI->GetMaster()->GetGUID() == challengerGuid;
    bool accepted = fromMaster || AI_VALUE2(uint8, "health", "self target") >= 90;

    WorldPacket packet(CMSG_DUEL_RESPONSE, 9);
    packet.WriteBit(challengerGuid[7]);
    packet.WriteBit(challengerGuid[1]);
    packet.WriteBit(challengerGuid[3]);
    packet.WriteBit(challengerGuid[4]);
    packet.WriteBit(challengerGuid[0]);
    packet.WriteBit(challengerGuid[2]);
    packet.WriteBit(challengerGuid[6]);
    packet.WriteBit(accepted);
    packet.WriteBit(challengerGuid[5]);

    packet.WriteByteSeq(challengerGuid[6]);
    packet.WriteByteSeq(challengerGuid[4]);
    packet.WriteByteSeq(challengerGuid[5]);
    packet.WriteByteSeq(challengerGuid[0]);
    packet.WriteByteSeq(challengerGuid[1]);
    packet.WriteByteSeq(challengerGuid[2]);
    packet.WriteByteSeq(challengerGuid[7]);
    packet.WriteByteSeq(challengerGuid[3]);

    bot->GetSession()->HandleDuelResponseOpcode(packet);

    if (accepted)
        botAI->ResetStrategies();

    return accepted;
}
