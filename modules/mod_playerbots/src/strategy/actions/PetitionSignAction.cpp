#include "PetitionSignAction.h"

#include "DatabaseEnv.h"
#include "Event.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotSecurity.h"
#include "Playerbots.h"
#include "WorldPacket.h"

namespace
{
// Parse SMSG_PETITION_SHOW_SIGNATURES as built in TrinityCore 5.4.8 (PetitionsHandler.cpp).
// We only need the petition GUID and the owner/inviter GUID.
bool ParsePetitionShowSignatures(WorldPacket& p, ObjectGuid& outPetitionGuid, ObjectGuid& outOwnerGuid)
{
    p.rpos(0);

    ObjectGuid petitionGuid;
    ObjectGuid ownerGuid;

    ownerGuid[1] = p.ReadBit();
    petitionGuid[3] = p.ReadBit();
    ownerGuid[3] = p.ReadBit();
    petitionGuid[4] = p.ReadBit();
    petitionGuid[0] = p.ReadBit();
    ownerGuid[7] = p.ReadBit();
    ownerGuid[5] = p.ReadBit();
    petitionGuid[1] = p.ReadBit();
    petitionGuid[5] = p.ReadBit();
    petitionGuid[7] = p.ReadBit();
    ownerGuid[0] = p.ReadBit();
    ownerGuid[6] = p.ReadBit();
    petitionGuid[6] = p.ReadBit();
    ownerGuid[2] = p.ReadBit();
    ownerGuid[4] = p.ReadBit();

    uint32 playerCount = p.ReadBits(21);

    // Signer guids (bits).
    ObjectGuid tmp;
    for (uint32 i = 0; i < playerCount; ++i)
    {
        tmp[2] = p.ReadBit();
        tmp[0] = p.ReadBit();
        tmp[4] = p.ReadBit();
        tmp[7] = p.ReadBit();
        tmp[5] = p.ReadBit();
        tmp[1] = p.ReadBit();
        tmp[6] = p.ReadBit();
        tmp[3] = p.ReadBit();
    }

    petitionGuid[2] = p.ReadBit();

    // Signer guid bytes + per-entry uint32.
    for (uint32 i = 0; i < playerCount; ++i)
    {
        p.ReadByteSeq(tmp[6]);
        p.ReadByteSeq(tmp[0]);
        p.ReadByteSeq(tmp[1]);
        p.ReadByteSeq(tmp[3]);
        p.ReadByteSeq(tmp[2]);
        p.ReadByteSeq(tmp[5]);
        p.ReadByteSeq(tmp[7]);
        p.ReadByteSeq(tmp[4]);
        p.read_skip<uint32>();
    }

    // Remaining owner/petition bytes + guildId (uint32).
    p.ReadByteSeq(petitionGuid[6]);
    p.ReadByteSeq(petitionGuid[5]);
    p.ReadByteSeq(petitionGuid[4]);
    p.ReadByteSeq(ownerGuid[4]);
    p.ReadByteSeq(petitionGuid[1]);
    p.read_skip<uint32>(); // guildId/petition counter
    p.ReadByteSeq(petitionGuid[2]);
    p.ReadByteSeq(petitionGuid[3]);
    p.ReadByteSeq(petitionGuid[7]);
    p.ReadByteSeq(ownerGuid[5]);
    p.ReadByteSeq(ownerGuid[6]);
    p.ReadByteSeq(ownerGuid[3]);
    p.ReadByteSeq(ownerGuid[7]);
    p.ReadByteSeq(ownerGuid[1]);
    p.ReadByteSeq(ownerGuid[0]);
    p.ReadByteSeq(petitionGuid[0]);
    p.ReadByteSeq(ownerGuid[2]);

    if (!petitionGuid)
        return false;

    outPetitionGuid = petitionGuid;
    outOwnerGuid = ownerGuid;
    return true;
}

void BuildPetitionSignPacket(WorldPacket& out, ObjectGuid petitionGuid)
{
    out.Initialize(CMSG_PETITION_SIGN);

    // Trinity handler expects a leading byte (unknown/unused) before the guid bitpack.
    out << uint8(0);

    // Bits order must match WorldSession::HandlePetitionSignOpcode.
    out.WriteBit(petitionGuid[4]);
    out.WriteBit(petitionGuid[2]);
    out.WriteBit(petitionGuid[0]);
    out.WriteBit(petitionGuid[1]);
    out.WriteBit(petitionGuid[5]);
    out.WriteBit(petitionGuid[3]);
    out.WriteBit(petitionGuid[6]);
    out.WriteBit(petitionGuid[7]);
    out.FlushBits();

    // Byte order must match ReadByteSeq order in the handler.
    out.WriteByteSeq(petitionGuid[6]);
    out.WriteByteSeq(petitionGuid[1]);
    out.WriteByteSeq(petitionGuid[7]);
    out.WriteByteSeq(petitionGuid[2]);
    out.WriteByteSeq(petitionGuid[5]);
    out.WriteByteSeq(petitionGuid[3]);
    out.WriteByteSeq(petitionGuid[0]);
    out.WriteByteSeq(petitionGuid[4]);
}

void BuildPetitionDeclinePacket(WorldPacket& out, ObjectGuid petitionGuid)
{
    out.Initialize(CMSG_PETITION_DECLINE);

    // Bits/bytes order must match WorldSession::HandlePetitionDeclineOpcode.
    out.WriteBit(petitionGuid[5]);
    out.WriteBit(petitionGuid[6]);
    out.WriteBit(petitionGuid[4]);
    out.WriteBit(petitionGuid[3]);
    out.WriteBit(petitionGuid[1]);
    out.WriteBit(petitionGuid[7]);
    out.WriteBit(petitionGuid[0]);
    out.WriteBit(petitionGuid[2]);
    out.FlushBits();

    out.WriteByteSeq(petitionGuid[6]);
    out.WriteByteSeq(petitionGuid[2]);
    out.WriteByteSeq(petitionGuid[1]);
    out.WriteByteSeq(petitionGuid[5]);
    out.WriteByteSeq(petitionGuid[0]);
    out.WriteByteSeq(petitionGuid[7]);
    out.WriteByteSeq(petitionGuid[4]);
    out.WriteByteSeq(petitionGuid[3]);
}
}

bool PetitionSignAction::Execute(Event event)
{
    WorldPacket p(event.getPacket());

    ObjectGuid petitionGuid;
    ObjectGuid ownerGuid;
    if (!ParsePetitionShowSignatures(p, petitionGuid, ownerGuid))
        return false;

    // MoP: only guild charter petitions are relevant (arena teams no longer exist).
    uint32 type = 0;
    if (QueryResult result = CharacterDatabase.PQuery("SELECT `type` FROM `petition` WHERE `petitionguid` = '%u'", petitionGuid.GetCounter()))
    {
        Field* fields = result->Fetch();
        type = fields[0].GetUInt32();
    }
    else
    {
        return false;
    }

    // Trinity uses 9 for guild charter; anything else is ignored.
    if (type != 9)
        return false;

    bool accept = true;

    if (bot->GetGuildId())
    {
        botAI->TellError("Sorry, I am in a guild already");
        accept = false;
    }
    if (bot->GetGuildIdInvited())
    {
        botAI->TellError("Sorry, I am invited to a guild already");
        accept = false;
    }

    Player* owner = ownerGuid ? ObjectAccessor::FindPlayer(ownerGuid) : nullptr;
    if (!owner || owner == bot)
        return false;

    if (!accept || !botAI->GetSecurity()->CheckLevelFor(PlayerbotSecurityLevel::PLAYERBOT_SECURITY_GUILD, false, owner, true))
    {
        WorldPacket data;
        BuildPetitionDeclinePacket(data, petitionGuid);
        bot->GetSession()->HandlePetitionDeclineOpcode(data);
        return false;
    }

    WorldPacket data;
    BuildPetitionSignPacket(data, petitionGuid);
    bot->GetSession()->HandlePetitionSignOpcode(data);
    bot->Say("Thanks for the invite!", LANG_UNIVERSAL);
    return true;
}
