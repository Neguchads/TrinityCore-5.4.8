/*
* This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "PlayerbotMgr.h"
#include "WorldThread/PlayerbotOperations.h"
#include "WorldThread/PlayerbotWorldThreadProcessor.h"

#include <cstdio>
#include <cstring>
#include <istream>
#include <string>

#include "BotFactory.h"
#include "Helper.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotLoginQueryHolder.h"
#include "CharacterCache.h"
#include "ChannelMgr.h"
#include "CharacterHandler.h"
#include "Common.h"
#include "Chat.h"
#include "Define.h"
#include "Group.h"
#include "GroupMgr.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "WorldSession.h"
#include "ChannelMgr.h"
#include "Log.h"
#include "PlayerbotDbStore.h"

PlayerbotHolder::PlayerbotHolder() : PlayerbotAIBase(false) {}

PlayerBotMap PlayerbotHolder::GetPlayerBotsSnapshot() const
{
    std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
    return playerBots;
}

bool PlayerbotHolder::IsBotLoading(ObjectGuid guid) const
{
    std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
    return botLoading.find(guid) != botLoading.end();
}

void PlayerbotHolder::MarkBotLoading(ObjectGuid guid)
{
    std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
    botLoading.insert(guid);
}

void PlayerbotHolder::UnmarkBotLoading(ObjectGuid guid)
{
    std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
    botLoading.erase(guid);
}

size_t PlayerbotHolder::GetBotLoadingCount() const
{
    std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
    return botLoading.size();
}

uint32 PlayerbotHolder::GetPlayerbotsCount() const
{
    std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
    return static_cast<uint32>(playerBots.size());
}

void PlayerbotHolder::AddPlayerBot(ObjectGuid playerGuid, uint32 masterAccountId)
{
    // bot is loading
    if (IsBotLoading(playerGuid))
    {
        TC_LOG_DEBUG("playerbots", "Bot %u is already loading", playerGuid.GetCounter());
        return;
    }

    // has bot already been added?
    Player* bot = ObjectAccessor::FindPlayer(playerGuid);
    if (bot && bot->IsInWorld())
    {
        TC_LOG_DEBUG("playerbots", "Bot %u is already in game", playerGuid.GetCounter());
        return;
    }

    uint32 accountId = sCharacterCache->GetCharacterAccountIdByGuid(playerGuid);
    if (!accountId)
    {
        TC_LOG_DEBUG("playerbots", "Bot %u has invalid accountid", playerGuid.GetCounter());
        return;
    }

    // will be delete in WorldSession
    std::shared_ptr<PlayerbotLoginQueryHolder> holder = std::make_shared<PlayerbotLoginQueryHolder>(this, masterAccountId, accountId, playerGuid);
    if (!holder->Initialize())
    {
        TC_LOG_ERROR("playerbots", "Holder initialize failed");
        return;
    }

    MarkBotLoading(playerGuid);

    sWorld->AddQueryHolderCallback(CharacterDatabase.DelayQueryHolder(holder)).AfterComplete([this](SQLQueryHolderBase const& holder)
    {
            HandlePlayerBotLoginCallback(static_cast<PlayerbotLoginQueryHolder const&>(holder));
    });
}

void PlayerbotHolder::HandlePlayerBotLoginCallback(PlayerbotLoginQueryHolder const& holder)
{
    uint32 botAccountId = holder.GetAccountId();
    ObjectGuid botGUID = holder.GetGuid();

    uint32 const playerLimit = sWorld->GetPlayerAmountLimit();
    if (playerLimit > 0 && sWorld->GetActiveSessionCount() >= playerLimit)
    {
        TC_LOG_INFO("playerbots", "Bot %u login delayed: player limit reached (%u).", botGUID.GetCounter(), playerLimit);
        UnmarkBotLoading(botGUID);
        return;
    }

    // At login DBC locale should be what the server is set to use by default (as spells etc are hardcoded to ENUS this
    // allows channels to work as intended)
    // Create a WorldSession for the bot
    WorldSession* botSession = new WorldSession(botAccountId, nullptr, AccountTypes::SEC_PLAYER, Expansions::EXPANSION_MISTS_OF_PANDARIA, 0, LOCALE_enUS, 0, 0, false, false, true);

    botSession->HandlePlayerLogin(holder);  // will delete lqh
    Player* bot = botSession->GetPlayer();
    if (!bot)
    {
        // Debug log
        TC_LOG_DEBUG("playerbots", "Bot player could not be loaded for account ID: %u", botAccountId);
        botSession->LogoutPlayer(true);
        delete botSession;
        UnmarkBotLoading(botGUID);
        return;
    }

    uint32 masterAccount = holder.GetMasterAccountId();
    WorldSession* masterSession = masterAccount ? sWorld->FindSession(masterAccount) : nullptr;

    // Check if masterSession->GetPlayer() is valid
    Player* masterPlayer = masterSession ? masterSession->GetPlayer() : nullptr;
    if (masterSession && !masterPlayer)
    {
        TC_LOG_DEBUG("playerbots", "Master session found but no player is associated for master account ID: %u", masterAccount);
    }

    std::ostringstream out;
    bool allowed = true;

    // Random-bot only: prefer configured random-bot accounts.
    // Also allow bots explicitly tracked in random-bot events (for async/account-discovery fallback).
    bool const inRandomAccounts = sPlayerbotAIConfig->IsInRandomAccountList(botAccountId);
    bool const hasRandomBotEvent = !masterAccount &&
        (sRandomPlayerbotMgr->GetValue(botGUID.GetCounter(), "add") || sRandomPlayerbotMgr->GetValue(botGUID.GetCounter(), "logout"));
    allowed = inRandomAccounts || hasRandomBotEvent ||
        (masterSession && masterSession->GetSecurity() >= SEC_GAMEMASTER);

    if (!allowed)
        out << "Failure: You are not allowed to control bot " << bot->GetName().c_str();

    if (allowed && masterSession && masterPlayer)
    {
        PlayerbotMgr* mgr = GET_PLAYERBOT_MGR(masterPlayer);
        if (!mgr)
        {
            TC_LOG_DEBUG("playerbots", "PlayerbotMgr not found for master player with GUID: %u", masterPlayer->GetGUID().GetCounter());
        }

        uint32 count = mgr->GetPlayerbotsCount();
        uint32 cls_count = mgr->GetPlayerbotsCountByClass(bot->GetClass());
        if (count >= sPlayerbotAIConfig->maxAddedBots)
        {
            allowed = false;
            out << "Failure: You have added too many bots";
        }
        else if (cls_count >= sPlayerbotAIConfig->maxAddedBotsPerClass)
        {
            allowed = false;
            out << "Failure: You have added too many bots for this class";
        }
    }

    if (allowed)
    {
        sRandomPlayerbotMgr->OnPlayerLogin(bot);
        OnBotLogin(bot);

        TC_LOG_DEBUG("playerbots", "Player logged: %s", bot->GetName().c_str());
    }
    else
    {
        TC_LOG_ERROR("playerbots", "Bot error on logging: %s", out.str().c_str());
        botSession->LogoutPlayer(true);
        delete botSession;
    }

    UnmarkBotLoading(botGUID);
}

void PlayerbotHolder::UpdateSessions()
{
    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator itr = bots.begin(); itr != bots.end(); ++itr)
    {
        Player* const bot = itr->second;
        if (bot->IsBeingTeleported())
        {
            PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
            if (botAI)
            {
                botAI->HandleTeleportAck();
            }
        }
        else if (bot->IsInWorld())
        {
            HandleBotPackets(bot->GetSession());
        }
    }
}

void PlayerbotHolder::HandleBotPackets(WorldSession* session)
{
    if (!session || !session->GetPlayer())
        return;

    session->HandleBotPackets();
}

void PlayerbotHolder::LogoutAllBots()
{
    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (auto& itr : bots)
    {
        Player* bot = itr.second;
        if (!bot)
            continue;

        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (!botAI || botAI->IsRealPlayer())
            continue;

        LogoutPlayerBot(bot->GetGUID());
    }
}

void PlayerbotMgr::CancelLogout()
{
    Player* master = GetMaster();
    if (!master)
        return;

    PlayerBotMap botMap = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = botMap.begin(); it != botMap.end(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (!botAI || botAI->IsRealPlayer())
            continue;

        if (bot->GetSession()->isLogingOut())
        {
            /*WorldPackets::Character::LogoutCancel data = WorldPacket(CMSG_LOGOUT_CANCEL);
            bot->GetSession()->HandleLogoutCancelOpcode(data);
            botAI->TellMaster("Logout cancelled!");*/
        }
    }

    PlayerBotMap randomBots = sRandomPlayerbotMgr->GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = randomBots.begin(); it != randomBots.end(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (!botAI || botAI->IsRealPlayer())
            continue;

        if (botAI->GetMaster() != master)
            continue;

        if (bot->GetSession()->isLogingOut())
        {
            /*WorldPackets::Character::LogoutCancel data = WorldPacket(CMSG_LOGOUT_CANCEL);
            bot->GetSession()->HandleLogoutCancelOpcode(data);*/
        }
    }
}

void PlayerbotHolder::LogoutPlayerBot(ObjectGuid guid)
{
    if (Player* bot = GetPlayerBot(guid))
    {
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (!botAI)
            return;

        // Queue group cleanup in world thread to avoid unsafe cross-thread access.
        auto cleanupOp = std::make_unique<BotLogoutGroupCleanupOperation>(guid);
        PlayerbotWorldThreadProcessor::instance().QueueOperation(std::move(cleanupOp));

        Group* group = bot->GetGroup();
        if (group && !bot->InBattleground() && !bot->InBattlegroundQueue() && botAI->HasActivePlayerMaster())
        {
            if (!sPlayerbotAIConfig->explicitDbStoreSave)
                sPlayerbotDbStore->Save(botAI, "");
        }

        TC_LOG_INFO("playerbots", "Bot %s logging out", bot->GetName().c_str());
        bot->SaveToDB(false);

        WorldSession* botWorldSessionPtr = bot->GetSession();
        WorldSession* masterWorldSessionPtr = nullptr;

        if (botWorldSessionPtr->isLogingOut())
            return;

        Player* master = botAI->GetMaster();
        if (master)
            masterWorldSessionPtr = master->GetSession();

        // check for instant logout
        bool logout = botWorldSessionPtr->ShouldLogOut(time(nullptr));

        if (masterWorldSessionPtr && masterWorldSessionPtr->ShouldLogOut(time(nullptr)))
            logout = true;

        if (masterWorldSessionPtr && !masterWorldSessionPtr->GetPlayer())
            logout = true;

        if (bot->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_RESTING) || bot->HasUnitState(UNIT_STATE_IN_FLIGHT) ||
            botWorldSessionPtr->GetSecurity() >= AccountTypes::SEC_PLAYER)
        {
            logout = true;
        }

        if (master &&
            (master->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_RESTING) || master->HasUnitState(UNIT_STATE_IN_FLIGHT) ||
                (masterWorldSessionPtr &&
                    masterWorldSessionPtr->GetSecurity() >= AccountTypes::SEC_PLAYER)))
        {
            logout = true;
        }

        //TravelTarget* target = nullptr;
        //if (botAI->GetAiObjectContext())  // Maybe some day re-write to delate all pointer values.
        {
            //target = botAI->GetAiObjectContext()->GetValue<TravelTarget*>("travel target")->Get();
        }

        // Peiru: Allow bots to always instant logout to see if this resolves logout crashes
        logout = true;

        // if no instant logout, request normal logout
        if (!logout)
        {
            if (bot->GetSession()->isLogingOut())
                return;
            else if (bot)
            {
                /*botAI->TellMaster("I'm logging out!");
                WorldPackets::Character::LogoutRequest data = WorldPacket(CMSG_LOGOUT_REQUEST);
                botWorldSessionPtr->HandleLogoutRequestOpcode(data);
                if (!bot)
                {
                    {
                        std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
                        playerBots.erase(guid);
                    }
                    delete botWorldSessionPtr;
                    if (target)
                        delete target;
                }*/
                return;
            }
            else
            {
                {
                    std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
                    playerBots.erase(guid);  // deletes bot player ptr inside this WorldSession PlayerBotMap
                }
                sWorld->DecreaseBotSessionCount();
                delete botWorldSessionPtr;  // finally delete the bot's WorldSession
                //if (target)
                    //delete target;
            }
            return;
        }  // if instant logout possible, do it
        else if (bot && (logout || !botWorldSessionPtr->isLogingOut()))
        {
            botAI->TellMaster("Goodbye!");
            {
                std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
                playerBots.erase(guid);  // deletes bot player ptr inside this WorldSession PlayerBotMap
            }
            sWorld->DecreaseBotSessionCount();
            sPlayerbotsMgr->RemovePlayerBotData(guid, true);
            botWorldSessionPtr->LogoutPlayer(true);  // this will delete the bot Player object and PlayerbotAI object
            delete botWorldSessionPtr;               // finally delete the bot's WorldSession
        }
    }
}

void PlayerbotHolder::DisablePlayerBot(ObjectGuid guid)
{
    if (Player* bot = GetPlayerBot(guid))
    {
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (!botAI)
        {
            return;
        }
        botAI->TellMaster("Goodbye!");
        bot->StopMoving();
        bot->GetMotionMaster()->Clear();

        Group* group = bot->GetGroup();
        if (group && !bot->InBattleground() && !bot->InBattlegroundQueue() && botAI->HasActivePlayerMaster())
        {
            if (!sPlayerbotAIConfig->explicitDbStoreSave)
                sPlayerbotDbStore->Save(botAI, "");
        }

        TC_LOG_DEBUG("playerbots", "Bot %s logged out", bot->GetName().c_str());

        bot->SaveToDB(false);

        //if (botAI->GetAiObjectContext())  // Maybe some day re-write to delate all pointer values.
        //{
        //    TravelTarget* target = botAI->GetAiObjectContext()->GetValue<TravelTarget*>("travel target")->Get();
        //    if (target)
        //        delete target;
        //}

        {
            std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
            playerBots.erase(guid);  // deletes bot player ptr inside this WorldSession PlayerBotMap
        }
        sWorld->DecreaseBotSessionCount();
        sPlayerbotsMgr->RemovePlayerBotData(guid, true);

        delete botAI;
    }
}

Player* PlayerbotHolder::GetPlayerBot(ObjectGuid::LowType lowGuid) const
{
    ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(lowGuid);
    std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
    PlayerBotMap::const_iterator it = playerBots.find(playerGuid);
    return (it == playerBots.end()) ? 0 : it->second;
}

Player* PlayerbotHolder::GetPlayerBot(ObjectGuid playerGuid) const
{
    std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
    PlayerBotMap::const_iterator it = playerBots.find(playerGuid);
    return (it == playerBots.end()) ? 0 : it->second;
}

void PlayerbotHolder::OnBotLogin(Player* const bot)
{
    // Prevent duplicate login
    {
        std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
        if (playerBots.find(bot->GetGUID()) != playerBots.end())
            return;
    }

    sPlayerbotsMgr->AddPlayerbotData(bot, true);
    {
        std::lock_guard<std::recursive_mutex> lock(_botStateMutex);
        playerBots[bot->GetGUID()] = bot;
    }
    sWorld->IncreaseBotSessionCount();
    OnBotLoginInternal(bot);

    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
    if (!botAI)
    {
        // Log a warning here to indicate that the botAI is null
        //TC_LOG_DEBUG("playerbots", "PlayerbotAI is null for bot with GUID: %u", bot->GetGUID());
        return;
    }

    Player* master = botAI->GetMaster();
    if (!master)
    {
        // Log a warning to indicate that the master is null
        //TC_LOG_DEBUG("playerbots", "Master is null for bot with GUID: %u", bot->GetGUID());
        return;
    }

    Group* group = bot->GetGroup();
    if (group)
    {
        bool groupValid = false;
        Group::MemberSlotList const& slots = group->GetMemberSlots();
        for (Group::MemberSlotList::const_iterator i = slots.begin(); i != slots.end(); ++i)
        {
            ObjectGuid member = i->guid;
            if (master)
            {
                if (master->GetGUID() == member)
                {
                    groupValid = true;
                    break;
                }
            }
            else
            {
                uint32 account = sCharacterCache->GetCharacterAccountIdByGuid(member);
                if (!sPlayerbotAIConfig->IsInRandomAccountList(account))
                {
                    groupValid = true;
                    break;
                }
            }
        }

        if (!groupValid)
        {
            bot->RemoveFromGroup();
        }
    }

    group = bot->GetGroup();
    if (group)
    {
        botAI->ResetStrategies();
    }
    else
    {
        botAI->ResetStrategies();
    }

    if (master && !master->HasUnitState(UNIT_STATE_IN_FLIGHT))
    {
        bot->GetMotionMaster()->MovementExpired();
        bot->CleanupAfterTaxiFlight();
    }

    // check activity
    botAI->AllowActivity(ALL_ACTIVITY, true);

    // set delay on login
    botAI->SetNextCheckDelay(urand(2000, 4000));

    botAI->TellMaster("Hello!");

    if (master && master->GetGroup() && !group)
    {
        Group* mgroup = master->GetGroup();
        if (mgroup->GetMembersCount() >= 5)
        {
            if (!mgroup->isRaidGroup() && !mgroup->isLFGGroup() && !mgroup->isBGGroup() && !mgroup->isBFGroup())
            {
                mgroup->ConvertToRaid();
            }
            if (mgroup->isRaidGroup())
            {
                mgroup->AddMember(bot);
            }
        }
        else
        {
            mgroup->AddMember(bot);
        }
    }
    else if (master && !group)
    {
        Group* newGroup = new Group();
        newGroup->Create(master);
        sGroupMgr->AddGroup(newGroup);
        newGroup->AddMember(bot);
    }

    if (master)
    {
        bot->TeleportTo(master->GetWorldLocation());
    }

    uint32 accountId = bot->GetSession()->GetAccountId();
    /*bool isRandomAccount = sPlayerbotAIConfig->IsInRandomAccountList(accountId);

    if (isRandomAccount && sPlayerbotAIConfig->randomBotFixedLevel)
    {
        bot->SetPlayerFlag(PLAYER_FLAGS_NO_XP_GAIN);
    }
    else if (isRandomAccount && !sPlayerbotAIConfig->randomBotFixedLevel)
    {
        bot->RemovePlayerFlag(PLAYER_FLAGS_NO_XP_GAIN);
    }*/

    bot->SaveToDB(false);
    /*if (master && isRandomAccount && master->getLevel() < bot->getLevel())
    {
        uint32 mixedGearScore =
            PlayerbotAI::GetMixedGearScore(master, true, false, 12) * sPlayerbotAIConfig->autoInitEquipLevelLimitRatio;
        PlayerbotFactory factory(bot, master->GetLevel(), ITEM_QUALITY_LEGENDARY, mixedGearScore);
        factory.Randomize(false);
    }

    // CMaNGOS has additional "grouper" personality logic to decide whether a bot should join
    // global/world chat. That system is not ported here, so we do not auto-join World.

    // join standard channels
    uint8 locale = BroadcastHelper::GetLocale();
    AreaTableEntry const* current_zone = GET_PLAYERBOT_AI(bot)->GetCurrentZone();
    ChannelMgr* cMgr = ChannelMgr::forTeam(bot->GetTeamId());
    std::string current_zone_name = current_zone ? GET_PLAYERBOT_AI(bot)->GetLocalizedAreaName(current_zone) : "";

    if (current_zone && cMgr)
    {
        for (uint32 i = 0; i < sChatChannelsStore.GetNumRows(); ++i)
        {
            ChatChannelsEntry const* channel = sChatChannelsStore.LookupEntry(i);
            if (!channel)
                continue;

            Channel* new_channel = nullptr;
            switch (channel->ChannelID)
            {
            case ChatChannelId::GENERAL:
            case ChatChannelId::LOCAL_DEFENSE:
            {
                char new_channel_name_buf[100];
                snprintf(new_channel_name_buf, 100, channel->pattern[locale], current_zone_name.c_str());
                new_channel = cMgr->GetJoinChannel(new_channel_name_buf, channel->ChannelID);
                break;
            }
            case ChatChannelId::TRADE:
            case ChatChannelId::GUILD_RECRUITMENT:
            {
                char new_channel_name_buf[100];
                //3459 is ID for a zone named "City" (only exists for the sake of using its name)
                //Currently in magons TBC, if you switch zones, then you join "Trade - <zone>" and "GuildRecruitment - <zone>"
                //which is a core bug, should be "Trade - City" and "GuildRecruitment - City" in both 1.12 and TBC
                //but if you (actual player) logout in a city and log back in - you join "City" versions
                snprintf(new_channel_name_buf, 100, channel->pattern[locale], GET_PLAYERBOT_AI(bot)->GetLocalizedAreaName(GetAreaEntryByAreaID(3459)).c_str());
                new_channel = cMgr->GetJoinChannel(new_channel_name_buf, channel->ChannelID);
                break;
            }
            case ChatChannelId::LOOKING_FOR_GROUP:
            case ChatChannelId::WORLD_DEFENSE:
            {
                new_channel = cMgr->GetJoinChannel(channel->pattern[locale], channel->ChannelID);
                break;
            }
            default:
                break;
            }

            if (new_channel)
                new_channel->JoinChannel(bot, "");
        }
    }*/
}

std::string const PlayerbotHolder::ProcessBotCommand(std::string const /*cmd*/, ObjectGuid /*guid*/, ObjectGuid /*masterguid*/,
    bool /*admin*/, uint32 /*masterAccountId*/, uint32 /*masterGuildId*/)
{
    return "unknown command";
}

bool PlayerbotMgr::HandlePlayerbotMgrCommand(ChatHandler* handler, char const* args)
{
    if (!sPlayerbotAIConfig->enabled)
    {
        handler->PSendSysMessage("|cffff0000Playerbot system is currently disabled!");
        return false;
    }

    WorldSession* m_session = handler->GetSession();
    if (!m_session)
    {
        handler->PSendSysMessage("You may only add bots from an active session");
        return false;
    }

    Player* player = m_session->GetPlayer();
    PlayerbotMgr* mgr = GET_PLAYERBOT_MGR(player);
    if (!mgr)
    {
        handler->PSendSysMessage("You cannot control bots yet");
        return false;
    }

    std::vector<std::string> messages = mgr->HandlePlayerbotCommand(args, player);
    if (messages.empty())
        return true;

    for (std::vector<std::string>::iterator i = messages.begin(); i != messages.end(); ++i)
    {
        handler->PSendSysMessage("%s", i->c_str());
    }

    return true;
}

std::vector<std::string> PlayerbotHolder::HandlePlayerbotCommand(char const* args, Player* master)
{
    std::vector<std::string> messages;

    if (!*args)
    {
        messages.push_back("usage: list/reload/tweak/self or add/init/remove PLAYERNAME\n");
        messages.push_back("usage: addclass CLASSNAME");
        return messages;
    }

    char* cmd = strtok((char*)args, " ");
    char* charname = strtok(nullptr, " ");
    if (!cmd)
    {
        messages.push_back("usage: list/reload/tweak/self or add/init/remove PLAYERNAME or addclass CLASSNAME");
        return messages;
    }

    auto parseState = [](char const* stateText) -> BotState
    {
        if (!stateText)
            return BOT_STATE_NON_COMBAT;

        if (!strcmp(stateText, "combat"))
            return BOT_STATE_COMBAT;
        if (!strcmp(stateText, "dead"))
            return BOT_STATE_DEAD;
        if (!strcmp(stateText, "noncombat") || !strcmp(stateText, "nc"))
            return BOT_STATE_NON_COMBAT;

        return BOT_STATE_NON_COMBAT;
    };

    if (!strcmp(cmd, "strategy") || (!strcmp(cmd, "add") && charname && !strcmp(charname, "strategy")))
    {
        char* strategyName = (!strcmp(cmd, "strategy")) ? charname : strtok(nullptr, " ");
        char* stateText = strtok(nullptr, " ");

        if (!strategyName)
        {
            messages.push_back("usage: add strategy <name> [combat|noncombat|dead]");
            return messages;
        }

        BotState state = parseState(stateText);
        std::string change = "+" + std::string(strategyName);

        uint32 applied = 0;
        if (master && master->GetTarget())
        {
            if (Player* target = ObjectAccessor::FindConnectedPlayer(master->GetTarget()))
            {
                if (PlayerbotAI* ai = GET_PLAYERBOT_AI(target))
                {
                    ai->ChangeStrategy(change, state);
                    applied++;
                }
            }
        }

        if (applied == 0)
        {
            PlayerBotMap bots = GetPlayerBotsSnapshot();
            for (PlayerBotMap::const_iterator it = bots.begin(); it != bots.end(); ++it)
            {
                if (Player* bot = it->second)
                {
                    if (PlayerbotAI* ai = GET_PLAYERBOT_AI(bot))
                    {
                        ai->ChangeStrategy(change, state);
                        applied++;
                    }
                }
            }
        }

        if (applied == 0)
            messages.push_back("No bots found to apply strategy.");
        else
            messages.push_back("Strategy added: " + std::string(strategyName));

        return messages;
    }

    if (!strcmp(cmd, "addspec") && master)
    {
        if (!charname || !strcmp(charname, "tank") || !strcmp(charname, "dps") || !strcmp(charname, "heal"))
        {
            messages.push_back(
                "addclass: invalid SPECNAME(tank/heal/dps)");
            return messages;
        }
        std::map<std::string, std::vector<uint8>> clazs = {
            {"tank", { 1, 2, 11, 6, 10 }},
            {"heal", { 2, 5, 7, 11, 10 }},
            {"dps", { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }}
        };
    }
    if (!strcmp(cmd, "setspec") && master && master->GetTarget())
    {
        uint32 tab = std::atoi(charname);
        WorldPacket p(CMSG_SET_PRIMARY_TALENT_TREE);
        p << tab;

        auto bot = ObjectAccessor::FindConnectedPlayer(master->GetTarget());
        if (bot)
        {
            bot->ResetTalents(true, true, true);
            bot->GetSession()->HandeSetTalentSpecialization(p);
            bot->ActivateSpec(0);
            BotFactory factory(bot, bot->GetLevel());
            factory.InitTalentsTree(false);
            factory.InitEquipment(true);
            if (bot->GetLevel() >= sPlayerbotAIConfig->minEnchantingBotLevel)
                factory.ApplyEnchantAndGemsNew();
            GET_PLAYERBOT_AI(bot)->ResetStrategies();
            GET_PLAYERBOT_AI(bot)->Reset(true);
        }
        return StringVector();
    }

    if (!strcmp(cmd, "addclass"))
    {
        if (!charname)
        {
            messages.push_back(
                "addclass: invalid CLASSNAME(warrior/paladin/hunter/rogue/priest/shaman/mage/warlock/druid/dk)");
            return messages;
        }
        uint8 claz;
        if (!strcmp(charname, "warrior"))
        {
            claz = 1;
        }
        else if (!strcmp(charname, "paladin"))
        {
            claz = 2;
        }
        else if (!strcmp(charname, "hunter"))
        {
            claz = 3;
        }
        else if (!strcmp(charname, "rogue"))
        {
            claz = 4;
        }
        else if (!strcmp(charname, "priest"))
        {
            claz = 5;
        }
        else if (!strcmp(charname, "shaman"))
        {
            claz = 7;
        }
        else if (!strcmp(charname, "mage"))
        {
            claz = 8;
        }
        else if (!strcmp(charname, "warlock"))
        {
            claz = 9;
        }
        else if (!strcmp(charname, "druid"))
        {
            claz = 11;
        }
        else if (!strcmp(charname, "dk"))
        {
            claz = 6;
        }
        else if (!strcmp(charname, "monk"))
        {
            claz = 10;
        }
        else
        {
            messages.push_back("Error: Invalid Class. Try again.");
            return messages;
        }
        TeamId teamId = master->GetTeamId();
        GuidVector guidCache = sRandomPlayerbotMgr->AddclassCache()[RandomPlayerbotMgr::GetTeamClassIdx(teamId == TEAM_ALLIANCE, claz)];
        
        std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
        std::shuffle(guidCache.begin(), guidCache.end(), gen);
        for (size_t i = 0; i < guidCache.size(); i++)
        {
            ObjectGuid guid = guidCache[i];
            if (IsBotLoading(guid))
                continue;
            if (ObjectAccessor::FindConnectedPlayer(guid))
                continue;

            AddPlayerBot(guid, master->GetSession()->GetAccountId());
            // ugly
            std::thread([this, master, guid]
            {
                Player* bot = nullptr;
                int max_try = 100;
                do
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(120));
                    bot = ObjectAccessor::FindConnectedPlayer(guid);
                    if (bot) break;
                    --max_try;
                } while (bot == nullptr && max_try > 0);
                if (!bot) return;
                sRandomPlayerbotMgr->SetValue(bot, "level", master->GetLevel());
                sRandomPlayerbotMgr->Randomize(bot);
                BotFactory factory(bot, bot->GetLevel());
                factory.InitTalentsTree(true);
                factory.InitEquipment(true);
                if (bot->GetLevel() >= sPlayerbotAIConfig->minEnchantingBotLevel)
                    factory.ApplyEnchantAndGemsNew();
            }).detach();
            
            messages.push_back("Add class " + std::string(charname));
            return messages;
        }
        messages.push_back("Add class failed, no available characters!");
        return messages;
    }

    return {};
}

uint32 PlayerbotHolder::GetAccountId(std::string const name) { return AccountMgr::GetId(name); }

uint32 PlayerbotHolder::GetAccountId(ObjectGuid guid)
{
    if (!guid.IsPlayer())
        return 0;

    // prevent DB access for online player
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetSession()->GetAccountId();

    ObjectGuid::LowType lowguid = guid.GetCounter();
    if (QueryResult result = CharacterDatabase.PQuery("SELECT account FROM characters WHERE guid = {}", lowguid))
    {
        uint32 acc = (*result)[0].GetUInt32();
        return acc;
    }

    return 0;
}

std::string const PlayerbotHolder::ListBots(Player* master)
{
    std::set<std::string> bots;
    std::map<uint8, std::string> classNames;

    classNames[CLASS_DEATH_KNIGHT] = "Death Knight";
    classNames[CLASS_DRUID] = "Druid";
    classNames[CLASS_HUNTER] = "Hunter";
    classNames[CLASS_MAGE] = "Mage";
    classNames[CLASS_PALADIN] = "Paladin";
    classNames[CLASS_PRIEST] = "Priest";
    classNames[CLASS_ROGUE] = "Rogue";
    classNames[CLASS_SHAMAN] = "Shaman";
    classNames[CLASS_WARLOCK] = "Warlock";
    classNames[CLASS_WARRIOR] = "Warrior";
    classNames[CLASS_DEATH_KNIGHT] = "DeathKnight";
    classNames[CLASS_MONK] = "Monk";

    std::map<std::string, std::string> online;
    std::vector<std::string> names;
    std::map<std::string, std::string> classes;

    PlayerBotMap botMap = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = botMap.begin(); it != botMap.end(); ++it)
    {
        Player* const bot = it->second;
        std::string const name = bot->GetName();
        bots.insert(name);

        names.push_back(name);
        online[name] = "+";
        classes[name] = classNames[bot->GetClass()];
    }

    /*if (master)
    {
        QueryResult results = CharacterDatabase.Query("SELECT class, name FROM characters WHERE account = {}",
            master->GetSession()->GetAccountId());
        if (results)
        {
            do
            {
                Field* fields = results->Fetch();
                uint8 cls = fields[0].Get<uint8>();
                std::string const name = fields[1].Get<std::string>();
                if (bots.find(name) == bots.end() && name != master->GetSession()->GetPlayerName())
                {
                    names.push_back(name);
                    online[name] = "-";
                    classes[name] = classNames[cls];
                }
            } while (results->NextRow());
        }
    }*/

    std::sort(names.begin(), names.end());

    if (Group* group = master->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player* member = ObjectAccessor::FindPlayer(itr->guid);
            if (member && sRandomPlayerbotMgr->IsRandomBot(member))
            {
                std::string const name = member->GetName();

                names.push_back(name);
                online[name] = "+";
                classes[name] = classNames[member->GetClass()];
            }
        }
    }

    std::ostringstream out;
    bool first = true;
    out << "Bot roster: ";
    for (std::vector<std::string>::iterator i = names.begin(); i != names.end(); ++i)
    {
        if (first)
            first = false;
        else
            out << ", ";

        std::string const name = *i;
        out << online[name] << name << " " << classes[name];
    }

    return out.str();
}

std::string const PlayerbotHolder::LookupBots(Player* master)
{
    std::list<std::string> messages;
    messages.push_back("Classes Available:");
    messages.push_back("|TInterface\\icons\\INV_Sword_27.png:25:25:0:-1|t Warrior");
    messages.push_back("|TInterface\\icons\\INV_Hammer_01.png:25:25:0:-1|t Paladin");
    messages.push_back("|TInterface\\icons\\INV_Weapon_Bow_07.png:25:25:0:-1|t Hunter");
    messages.push_back("|TInterface\\icons\\INV_ThrowingKnife_04.png:25:25:0:-1|t Rogue");
    messages.push_back("|TInterface\\icons\\INV_Staff_30.png:25:25:0:-1|t Priest");
    messages.push_back("|TInterface\\icons\\inv_jewelry_talisman_04.png:25:25:0:-1|t Shaman");
    messages.push_back("|TInterface\\icons\\INV_staff_30.png:25:25:0:-1|t Mage");
    messages.push_back("|TInterface\\icons\\INV_staff_30.png:25:25:0:-1|t Warlock");
    messages.push_back("|TInterface\\icons\\Ability_Druid_Maul.png:25:25:0:-1|t Druid");
    messages.push_back("DK");
    messages.push_back("Monk");
    messages.push_back("(Usage: .bot lookup CLASS)");
    std::string ret_msg;
    for (std::string msg : messages)
    {
        ret_msg += msg + "\n";
    }
    return ret_msg;
}

uint32 PlayerbotHolder::GetPlayerbotsCountByClass(uint32 cls)
{
    uint32 count = 0;
    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = bots.begin(); it != bots.end(); ++it)
    {
        Player* const bot = it->second;
        if (bot && bot->IsInWorld() && bot->GetClass() == cls)
        {
            count++;
        }
    }
    return count;
}

PlayerbotMgr::PlayerbotMgr(Player* const master) : PlayerbotHolder(), master(master), lastErrorTell(0) {}

PlayerbotMgr::~PlayerbotMgr()
{
    if (master)
        sPlayerbotsMgr->RemovePlayerBotData(master->GetGUID(), false);
}

void PlayerbotMgr::UpdateAIInternal(uint32 elapsed, bool /*minimal*/)
{
    SetNextCheckDelay(sPlayerbotAIConfig->reactDelay);
    CheckTellErrors(elapsed);
}

void PlayerbotMgr::HandleCommand(uint32 type, std::string const text)
{
    Player* master = GetMaster();
    if (!master)
        return;

    if (text.find(":"/*sPlayerbotAIConfig->commandSeparator*/) != std::string::npos)
    {
        std::vector<std::string> commands;
        split(commands, text, ":"/*sPlayerbotAIConfig->commandSeparator.c_str()*/);
        for (std::vector<std::string>::iterator i = commands.begin(); i != commands.end(); ++i)
        {
            HandleCommand(type, *i);
        }

        return;
    }

    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = bots.begin(); it != bots.end(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        //if (botAI)
            //botAI->HandleCommand(type, text, master);
    }

    PlayerBotMap randomBots = sRandomPlayerbotMgr->GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = randomBots.begin(); it != randomBots.end(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        //if (botAI && botAI->GetMaster() == master)
            //botAI->HandleCommand(type, text, master);
    }
}

void PlayerbotMgr::HandleMasterIncomingPacket(WorldPacket const& packet)
{
    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = bots.begin(); it != bots.end(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (botAI)
            botAI->HandleMasterIncomingPacket(packet);
    }

    PlayerBotMap randomBots = sRandomPlayerbotMgr->GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = randomBots.begin(); it != randomBots.end(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (botAI && botAI->GetMaster() == GetMaster())
            botAI->HandleMasterIncomingPacket(packet);
    }

    switch (packet.GetOpcode())
    {
        // if master is logging out, log out all bots
    case CMSG_LOGOUT_REQUEST:
    {
        LogoutAllBots();
        break;
    }
    // if master cancelled logout, cancel too
    case CMSG_LOGOUT_CANCEL:
    {
        CancelLogout();
        break;
    }
    }
}

void PlayerbotMgr::HandleMasterOutgoingPacket(WorldPacket const& packet)
{
    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = bots.begin(); it != bots.end(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (botAI)
            botAI->HandleMasterOutgoingPacket(packet);
    }

    PlayerBotMap randomBots = sRandomPlayerbotMgr->GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = randomBots.begin(); it != randomBots.end(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (botAI && botAI->GetMaster() == GetMaster())
            botAI->HandleMasterOutgoingPacket(packet);
    }
}

void PlayerbotMgr::SaveToDB()
{
    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = bots.begin(); it != bots.end(); ++it)
    {
        Player* const bot = it->second;
        bot->SaveToDB(false);
    }

    PlayerBotMap randomBots = sRandomPlayerbotMgr->GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = randomBots.begin(); it != randomBots.end(); ++it)
    {
        Player* const bot = it->second;
        if (GET_PLAYERBOT_AI(bot) && GET_PLAYERBOT_AI(bot)->GetMaster() == GetMaster())
            bot->SaveToDB(false);
    }
}

void PlayerbotMgr::OnBotLoginInternal(Player* const bot)
{
    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
    if (!botAI)
    {
        return;
    }
    botAI->SetMaster(master);
    //botAI->ResetStrategies();
    sPlayerbotDbStore->Load(botAI, "");

    TC_LOG_INFO("playerbots", "Bot %s logged in - Active spec tab: %u Spec: %u ", bot->GetName().c_str(), (uint32)bot->GetActiveSpec(), (uint32)bot->GetSpecialization());
}

void PlayerbotMgr::OnPlayerLogin(Player* player)
{
    /*
    // set locale priority for bot texts
    sPlayerbotTextMgr->AddLocalePriority(player->GetSession()->GetSessionDbcLocale());

    if (sPlayerbotAIConfig->selfBotLevel > 2)
        HandlePlayerbotCommand("self", player);

    if (!sPlayerbotAIConfig->botAutologin)
        return;

    uint32 accountId = player->GetSession()->GetAccountId();
    QueryResult results = CharacterDatabase.Query("SELECT name FROM characters WHERE account = {}", accountId);
    if (results)
    {
        std::ostringstream out;
        out << "add ";
        bool first = true;
        do
        {
            Field* fields = results->Fetch();

            if (first)
                first = false;
            else
                out << ",";

            out << fields[0].Get<std::string>();
        } while (results->NextRow());

        HandlePlayerbotCommand(out.str().c_str(), player);
    }*/
}

void PlayerbotMgr::TellError(std::string const botName, std::string const text)
{
    std::lock_guard<std::mutex> lock(_errorsMutex);
    std::set<std::string> names = errors[text];
    if (names.find(botName) == names.end())
    {
        names.insert(botName);
    }
    TC_LOG_DEBUG("playerbots", "TellErro: %s: %s", botName.c_str(), text.c_str());
    errors[text] = names;
}

void PlayerbotMgr::CheckTellErrors(uint32 elapsed)
{
    time_t now = time(nullptr);
    if ((now - lastErrorTell) < sPlayerbotAIConfig->errorDelay / 1000)
        return;

    lastErrorTell = now;

    PlayerBotErrorMap errorsSnapshot;
    {
        std::lock_guard<std::mutex> lock(_errorsMutex);
        errorsSnapshot = errors;
        errors.clear();
    }

    for (PlayerBotErrorMap::iterator i = errorsSnapshot.begin(); i != errorsSnapshot.end(); ++i)
    {
        std::string const text = i->first;
        std::set<std::string> names = i->second;

        std::ostringstream out;
        bool first = true;
        for (std::set<std::string>::iterator j = names.begin(); j != names.end(); ++j)
        {
            if (!first)
                out << ", ";
            else
                first = false;

            out << *j;
        }

        out << "|cfff00000: " << text;

        //ChatHandler(master->GetSession()).PSendSysMessage(out.str().c_str());
    }

}

void PlayerbotsMgr::AddPlayerbotData(Player* player, bool isBotAI)
{
    if (!player)
    {
        return;
    }

    if (!isBotAI)
    {
        PlayerbotMgr* playerbotMgr = new PlayerbotMgr(player);
        {
            std::unique_lock<std::shared_mutex> lock(_dataMutex);
            std::unordered_map<ObjectGuid, PlayerbotAIBase*>::iterator itr = _playerbotsMgrMap.find(player->GetGUID());
            if (itr != _playerbotsMgrMap.end())
                _playerbotsMgrMap.erase(itr);

            ASSERT(_playerbotsMgrMap.emplace(player->GetGUID(), playerbotMgr).second);
        }

        playerbotMgr->OnPlayerLogin(player);
    }
    else
    {
        PlayerbotAI* botAI = new PlayerbotAI(player);
        std::unique_lock<std::shared_mutex> lock(_dataMutex);
        std::unordered_map<ObjectGuid, PlayerbotAIBase*>::iterator itr = _playerbotsAIMap.find(player->GetGUID());
        if (itr != _playerbotsAIMap.end())
            _playerbotsAIMap.erase(itr);

        ASSERT(_playerbotsAIMap.emplace(player->GetGUID(), botAI).second);
    }
}

void PlayerbotsMgr::RemovePlayerBotData(ObjectGuid const& guid, bool is_AI)
{
    std::unique_lock<std::shared_mutex> lock(_dataMutex);

    if (is_AI)
    {
        std::unordered_map<ObjectGuid, PlayerbotAIBase*>::iterator itr = _playerbotsAIMap.find(guid);
        if (itr != _playerbotsAIMap.end())
        {
            _playerbotsAIMap.erase(itr);
        }
    }
    else
    {
        std::unordered_map<ObjectGuid, PlayerbotAIBase*>::iterator itr = _playerbotsMgrMap.find(guid);
        if (itr != _playerbotsMgrMap.end())
        {
            _playerbotsMgrMap.erase(itr);
        }
    }
}

PlayerbotAI* PlayerbotsMgr::GetPlayerbotAI(Player* player)
{
    if (!(sPlayerbotAIConfig->enabled) || !player)
    {
        return nullptr;
    }
    std::shared_lock<std::shared_mutex> lock(_dataMutex);
    // if (player->GetSession()->isLogingOut() || player->IsDuringRemoveFromWorld()) {
    //     return nullptr;
    // }
    auto itr = _playerbotsAIMap.find(player->GetGUID());
    if (itr != _playerbotsAIMap.end())
    {
        if (itr->second->IsBotAI())
            return reinterpret_cast<PlayerbotAI*>(itr->second);
    }

    return nullptr;
}

PlayerbotMgr* PlayerbotsMgr::GetPlayerbotMgr(Player* player)
{
    if (!(sPlayerbotAIConfig->enabled) || !player)
    {
        return nullptr;
    }
    std::shared_lock<std::shared_mutex> lock(_dataMutex);
    auto itr = _playerbotsMgrMap.find(player->GetGUID());
    if (itr != _playerbotsMgrMap.end())
    {
        if (!itr->second->IsBotAI())
            return reinterpret_cast<PlayerbotMgr*>(itr->second);
    }

    return nullptr;
}
