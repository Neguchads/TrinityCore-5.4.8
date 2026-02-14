#include "GuildTriggers.h"

#include "Playerbots.h"

#include "Guild.h"
#include "GuildMgr.h"
#include "ObjectAccessor.h"

bool LeaveLargeGuildTrigger::IsActive()
{
    if (!bot || !bot->GetGuildId())
        return false;

    if (botAI->HasActivePlayerMaster())
        return false;

    if (botAI->IsAlt())
        return false;

    Guild* guild = sGuildMgr->GetGuildById(bot->GetGuildId());
    if (!guild)
        return false;

    // Hard overflow guard (matches CMaNGOS intent).
    if (guild->GetMembersCount() >= 1000)
        return true;

    if (!sPlayerbotAIConfig->randomBotGuildNearby)
        return false;

    if (botAI->IsInRealGuild())
        return false;

    // Only leave if guild leader is a bot (avoid leaving player-led guilds).
    Player* leader = ObjectAccessor::FindPlayer(guild->GetLeaderGUID());
    if (!leader)
        return false;

    PlayerbotAI* leaderAI = GET_PLAYERBOT_AI(leader);
    if (!leaderAI || leaderAI->IsRealPlayer())
        return false;

    uint32 members = guild->GetMembersCount();
    uint32 maxMembers = botAI->GetMaxPreferedGuildSize();
    return members > maxMembers;
}
