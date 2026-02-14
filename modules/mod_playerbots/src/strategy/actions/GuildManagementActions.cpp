#include "GuildManagementActions.h"

#include "Playerbots.h"

#include "Guild.h"
#include "GuildMgr.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "WorldSession.h"

namespace
{
Player* ResolvePlayerTarget(Player* bot, Event const& event)
{
    ObjectGuid guid = event.getObject();
    if (guid && guid.IsPlayer())
        if (Player* p = ObjectAccessor::FindPlayer(guid))
            return p;

    std::string name = event.getParam();
    if (!name.empty())
    {
        if (normalizePlayerName(name))
            if (Player* p = ObjectAccessor::FindPlayerByName(name))
                return p;
        return nullptr;
    }

    // Best-effort: if the requester is the master, use bot's current selection.
    if (bot)
        if (Player* selected = bot->GetSelectedPlayer())
            return selected;

    return event.getOwner();
}

void BuildGuildInvitePacket(WorldPacket& data, std::string const& name)
{
    data.Initialize(CMSG_GUILD_INVITE);
    data.WriteBits(name.size(), 9);
    data.FlushBits();
    data.WriteString(name);
}

void BuildGuildRemovePacket(WorldPacket& data, ObjectGuid targetGuid)
{
    data.Initialize(CMSG_GUILD_REMOVE);

    data.WriteBit(targetGuid[7]);
    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(targetGuid[5]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(targetGuid[0]);
    data.FlushBits();

    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(targetGuid[4]);
    data.WriteByteSeq(targetGuid[3]);
}

void BuildGuildSetMasterPacket(WorldPacket& data, std::string const& name)
{
    data.Initialize(CMSG_GUILD_SET_GUILD_MASTER);
    data.WriteBits(name.size(), 9);
    data.FlushBits();
    data.WriteString(name);
}

void BuildGuildPromotePacket(WorldPacket& data, ObjectGuid targetGuid)
{
    data.Initialize(CMSG_GUILD_PROMOTE);

    data.WriteBit(targetGuid[6]);
    data.WriteBit(targetGuid[0]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(targetGuid[5]);

    data.FlushBits();

    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(targetGuid[4]);
    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(targetGuid[6]);
}

void BuildGuildDemotePacket(WorldPacket& data, ObjectGuid targetGuid)
{
    data.Initialize(CMSG_GUILD_DEMOTE);

    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(targetGuid[0]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(targetGuid[5]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[1]);

    data.FlushBits();

    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(targetGuid[4]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(targetGuid[6]);
}
}

bool GuildManageNearbyAction::isUseful()
{
    if (!bot || !bot->GetGuildId())
        return false;

    // Don't let alts of active players auto-manage guilds.
    if (botAI->HasActivePlayerMaster() || botAI->IsAlt())
        return false;

    Guild* guild = sGuildMgr->GetGuildById(bot->GetGuildId());
    if (!guild)
        return false;

    return true;
}

bool GuildManageNearbyAction::Execute(Event /*event*/)
{
    Guild* guild = sGuildMgr->GetGuildById(bot->GetGuildId());
    if (!guild)
        return false;

    GuidVector nearGuids = AI_VALUE(GuidVector, "nearest friendly players");
    uint32 invited = 0;

    for (ObjectGuid const& guid : nearGuids)
    {
        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player || player == bot)
            continue;

        if (player->isDND())
            continue;

        uint32 guildSize = guild->GetMembersCount();

        // Promote/demote nearby guild members based on chance + bot's death count (CMaNGOS parity).
        if (player->GetGuildId() == bot->GetGuildId())
        {
            uint32 dCount = AI_VALUE(uint32, "death count");

            if (urand(0, 30) == 0 && dCount < 2 &&
                (bot->GetRank() + 1) < player->GetRank())
            {
                Event e("guild management", guid, player);
                botAI->DoSpecificAction("guild promote", e, true);
                continue;
            }

            if (urand(0, 30) == 0 && dCount > 2 &&
                bot->GetRank() < player->GetRank())
            {
                Event e("guild management", guid, player);
                botAI->DoSpecificAction("guild demote", e, true);
                continue;
            }

            continue;
        }

        if (player->GetGuildId() || player->GetGuildIdInvited())
            continue;

        if (!sPlayerbotAIConfig->randomBotGuildNearby)
            return false;

        if (guildSize >= sPlayerbotAIConfig->guildMaxBotLimit)
            return false;

        if (guildSize >= botAI->GetMaxPreferedGuildSize())
            continue;

        if (!sPlayerbotAIConfig->randomBotInvitePlayer)
        {
            if (PlayerbotAI* otherAI = GET_PLAYERBOT_AI(player))
            {
                if (otherAI->IsRealPlayer())
                    continue;
            }
            else
            {
                // Real player.
                continue;
            }
        }

        if (PlayerbotAI* otherAI = GET_PLAYERBOT_AI(player))
        {
            if (otherAI->GetGuilderType() == GuilderType::SOLO)
                continue;

            if (otherAI->HasActivePlayerMaster() && !sRandomPlayerbotMgr->IsRandomBot(player))
                continue;

            if (guildSize >= otherAI->GetMaxPreferedGuildSize() ||
                guildSize < otherAI->GetMaxPreferedGuildSize() / 4)
                continue;
        }

        // Require close distance unless already grouped.
        bool sameGroup = bot->GetGroup() && bot->GetGroup()->IsMember(player->GetGUID());
        if (!sameGroup && bot->GetDistance2d(player) > sPlayerbotAIConfig->spellDistance)
            continue;

        if (sPlayerbotAIConfig->inviteChat &&
            bot->GetDistance2d(player) < sPlayerbotAIConfig->spellDistance &&
            (sRandomPlayerbotMgr->IsRandomBot(bot) || !botAI->HasActivePlayerMaster()))
        {
            bot->Say("Join my guild?", LANG_UNIVERSAL);
        }

        Event e("guild management", guid, player);
        if (botAI->DoSpecificAction("guild invite", e, true))
            ++invited;

        if (invited >= 1) // keep it conservative per tick
            break;
    }

    return invited > 0;
}

bool GuildInviteAction::Execute(Event event)
{
    if (!bot || !bot->GetSession())
        return false;

    Player* player = ResolvePlayerTarget(bot, event);
    if (!player || player == bot)
        return false;

    Guild* guild = bot->GetGuild();
    if (!guild)
        return false;

    WorldPacket data;
    BuildGuildInvitePacket(data, player->GetName());
    bot->GetSession()->HandleGuildInviteOpcode(data);
    return true;
}

bool GuildJoinAction::Execute(Event event)
{
    if (!bot || !bot->GetSession())
        return false;

    if (bot->GetGuildId() || bot->GetGuildIdInvited())
        return false;

    Player* inviter = ResolvePlayerTarget(bot, event);
    if (!inviter || inviter == bot || !inviter->GetSession())
        return false;

    if (!inviter->GetGuildId())
        return false;

    // CMaNGOS parity: "guild join" is implemented by making the selected player invite the bot.
    WorldPacket data;
    BuildGuildInvitePacket(data, bot->GetName());
    inviter->GetSession()->HandleGuildInviteOpcode(data);
    return true;
}

bool GuildPromoteAction::Execute(Event event)
{
    if (!bot || !bot->GetSession())
        return false;

    Player* player = ResolvePlayerTarget(bot, event);
    if (!player || player == bot)
        return false;

    Guild* guild = bot->GetGuild();
    if (!guild)
        return false;

    if (player->GetGuildId() != bot->GetGuildId())
        return false;

    // Don't try to promote above/at our own rank.
    if (player->GetRank() <= 1 || bot->GetRank() >= static_cast<uint8>(player->GetRank() - 1))
        return false;

    WorldPacket data;
    BuildGuildPromotePacket(data, player->GetGUID());
    bot->GetSession()->HandleGuildPromoteOpcode(data);
    return true;
}

bool GuildDemoteAction::Execute(Event event)
{
    if (!bot || !bot->GetSession())
        return false;

    Player* player = ResolvePlayerTarget(bot, event);
    if (!player || player == bot)
        return false;

    Guild* guild = bot->GetGuild();
    if (!guild)
        return false;

    if (player->GetGuildId() != bot->GetGuildId())
        return false;

    if (bot->GetRank() >= player->GetRank())
        return false;

    WorldPacket data;
    BuildGuildDemotePacket(data, player->GetGUID());
    bot->GetSession()->HandleGuildDemoteOpcode(data);
    return true;
}

bool GuildRemoveAction::Execute(Event event)
{
    if (!bot || !bot->GetSession())
        return false;

    Player* player = ResolvePlayerTarget(bot, event);
    if (!player || player == bot)
        return false;

    if (!bot->GetGuildId() || player->GetGuildId() != bot->GetGuildId())
        return false;

    WorldPacket data;
    BuildGuildRemovePacket(data, player->GetGUID());
    bot->GetSession()->HandleGuildRemoveOpcode(data);
    return true;
}

bool GuildLeaderAction::Execute(Event event)
{
    if (!bot || !bot->GetSession())
        return false;

    Player* player = ResolvePlayerTarget(bot, event);
    if (!player || player == bot)
        return false;

    if (!bot->GetGuildId() || player->GetGuildId() != bot->GetGuildId())
        return false;

    WorldPacket data;
    BuildGuildSetMasterPacket(data, player->GetName());
    bot->GetSession()->HandleGuildSetGuildMaster(data);
    return true;
}

bool GuildLeaveAction::Execute(Event /*event*/)
{
    if (!bot || !bot->GetSession())
        return false;

    if (!bot->GetGuildId())
        return false;

    WorldPacket data(CMSG_GUILD_LEAVE);
    bot->GetSession()->HandleGuildLeaveOpcode(data);
    return true;
}
