#include "PlayerbotSecurity.h"

#include <sstream>

#include "Chat.h"
#include "Group.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "World.h"
#include "PlayerbotAIConfig.h"

PlayerbotSecurity::PlayerbotSecurity(Player* const bot) : bot(bot)
{
    if (bot && bot->GetSession())
        account = bot->GetSession()->GetAccountId();
}

PlayerbotSecurityLevel PlayerbotSecurity::LevelFor(Player* from, DenyReason* reason, bool ignoreGroup)
{
    if (reason)
        *reason = DenyReason::PLAYERBOT_DENY_NONE;

    if (!bot || !from || from == bot)
        return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_DENY_ALL;

    // GM accounts can do everything.
    if (from->GetSession() && from->GetSession()->GetSecurity() >= SEC_GAMEMASTER)
        return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL;

    PlayerbotAI* ai = GET_PLAYERBOT_AI(bot);
    if (ai && ai->GetMaster() && from == ai->GetMaster())
        return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL;

    // Opposing factions.
    if (ai && ai->IsOpposing(from) && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP))
    {
        if (reason)
            *reason = DenyReason::PLAYERBOT_DENY_OPPOSING;
        return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_DENY_ALL;
    }

    // If requester is already grouped with the bot, allow.
    if (!ignoreGroup)
        if (Group* g = from->GetGroup())
            if (g->IsMember(bot->GetGUID()))
                return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL;

    // If bot is not grouped, allow invite-level actions (so it can be recruited).
    Group* botGroup = bot->GetGroup();
    if (!botGroup)
    {
        // Don't allow far-away invites.
        if (bot->GetMapId() != from->GetMapId() || bot->GetDistance(from) > sPlayerbotAIConfig->whisperDistance)
        {
            if (reason)
                *reason = DenyReason::PLAYERBOT_DENY_FAR;
            return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_TALK;
        }

        if (bot->InBattlegroundQueue())
        {
            if (reason)
                *reason = DenyReason::PLAYERBOT_DENY_BG;
            return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_TALK;
        }

        if (bot->IsUsingLfg(false))
        {
            if (reason)
                *reason = DenyReason::PLAYERBOT_DENY_LFG;
            return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_TALK;
        }

        if (reason)
            *reason = DenyReason::PLAYERBOT_DENY_INVITE;
        return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_INVITE;
    }

    // If bot is in a full group, allow only guild-level interactions (best-effort parity with cmangos).
    if (botGroup->IsFull())
    {
        if (reason)
            *reason = DenyReason::PLAYERBOT_DENY_FULL_GROUP;
        return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_GUILD;
    }

    // If bot isn't leader, don't allow invites.
    if (botGroup->GetLeaderGUID() != bot->GetGUID())
    {
        if (reason)
            *reason = DenyReason::PLAYERBOT_DENY_INVITE;
        return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_GUILD;
    }

    // Bot in LFG/BG should not accept new invites unless guildmate.
    if (bot->InBattlegroundQueue())
    {
        if (reason)
            *reason = DenyReason::PLAYERBOT_DENY_BG;
        return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_TALK;
    }

    if (bot->IsUsingLfg(false))
    {
        if (reason)
            *reason = DenyReason::PLAYERBOT_DENY_LFG;
        return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_TALK;
    }

    if (reason)
        *reason = DenyReason::PLAYERBOT_DENY_INVITE;
    return PlayerbotSecurityLevel::PLAYERBOT_SECURITY_INVITE;
}

bool PlayerbotSecurity::CheckLevelFor(PlayerbotSecurityLevel level, bool /*silent*/, Player* from, bool ignoreGroup)
{
    DenyReason r = DenyReason::PLAYERBOT_DENY_NONE;
    PlayerbotSecurityLevel have = LevelFor(from, &r, ignoreGroup);
    if (static_cast<uint8>(have) >= static_cast<uint8>(level) || from == bot)
        return true;

    // Don't whisper security errors to bots.
    if (PlayerbotAI* fromAI = GET_PLAYERBOT_AI(from))
        if (!fromAI->IsRealPlayer())
            return false;

    std::ostringstream out;
    switch (have)
    {
        case PlayerbotSecurityLevel::PLAYERBOT_SECURITY_DENY_ALL:
            out << "I'm kind of busy now.";
            break;
        case PlayerbotSecurityLevel::PLAYERBOT_SECURITY_TALK:
            switch (r)
            {
                case DenyReason::PLAYERBOT_DENY_OPPOSING:
                    out << "You are the enemy.";
                    break;
                case DenyReason::PLAYERBOT_DENY_BG:
                    out << "I'm queued for BG. Ask later.";
                    break;
                case DenyReason::PLAYERBOT_DENY_LFG:
                    out << "I'm queued for dungeon. Ask later.";
                    break;
                case DenyReason::PLAYERBOT_DENY_FAR:
                    out << "You must be closer to invite me.";
                    break;
                case DenyReason::PLAYERBOT_DENY_NOT_YOURS:
                default:
                    out << "I have a master already.";
                    break;
            }
            break;
        case PlayerbotSecurityLevel::PLAYERBOT_SECURITY_INVITE:
            out << "Invite me to your group first.";
            break;
        case PlayerbotSecurityLevel::PLAYERBOT_SECURITY_GUILD:
        default:
            out << "I can't do that right now.";
            break;
    }

    std::string text = out.str();
    if (!text.empty())
    {
        uint64 guid = from->GetGUID().GetRawValue();
        time_t now = time(nullptr);
        time_t lastSaid = whispers[guid][text];
        if (!lastSaid || (now - lastSaid) >= sPlayerbotAIConfig->repeatDelay / 1000)
        {
            whispers[guid][text] = now;
            bot->Whisper(text, bot->GetTeamId() == TEAM_ALLIANCE ? LANG_COMMON : LANG_ORCISH, from);
        }
    }

    return false;
}
