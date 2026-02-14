#include "GuildAcceptAction.h"

#include "Event.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotSecurity.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "WorldPacket.h"

bool GuildAcceptAction::Execute(Event event)
{
    Player* inviter = nullptr;

    // Trinity core stores the inviter GUID when sending SMSG_GUILD_INVITE.
    ObjectGuid inviterGuid = bot->GetLastGuildInviterGUID();
    if (inviterGuid)
        inviter = ObjectAccessor::FindPlayer(inviterGuid);

    if (!inviter)
        return false;

    bool accept = true;

    uint32 guildId = inviter->GetGuildId();
    if (!guildId)
    {
        botAI->TellError("You are not in a guild!");
        accept = false;
    }
    else if (bot->GetGuildId())
    {
        botAI->TellError("Sorry, I am in a guild already");
        accept = false;
    }
    else if (!botAI->GetSecurity()->CheckLevelFor(PlayerbotSecurityLevel::PLAYERBOT_SECURITY_GUILD, false, inviter, true))
    {
        botAI->TellError("Sorry, I don't want to join your guild :(");
        accept = false;
    }

    if (accept)
    {
        if (Guild* guild = sGuildMgr->GetGuildById(guildId))
        {
            // CMaNGOS parity: refuse to join extremely large guilds (avoid hard client/server limits).
            if (guild->GetMembersCount() > 1000)
            {
                botAI->TellError("This guild has over 1000 members. I refuse to join it.");
                accept = false;
            }
        }
    }

    if (accept && sPlayerbotAIConfig->inviteChat && bot->GetDistance(inviter) < sPlayerbotAIConfig->spellDistance * 1.5f &&
        GET_PLAYERBOT_AI(inviter) && (sRandomPlayerbotMgr->IsRandomBot(bot) || !botAI->HasActivePlayerMaster()))
    {
        bot->Say("Sounds good, sign me up!", LANG_UNIVERSAL);
    }

    WorldPacket dummy;
    if (accept)
        bot->GetSession()->HandleGuildAcceptOpcode(dummy);
    else
        bot->GetSession()->HandleGuildDeclineOpcode(dummy);

    return true;
}
