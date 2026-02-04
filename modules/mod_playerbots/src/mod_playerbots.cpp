/*
** Made by Traesh https://github.com/Traesh
** AzerothCore 2019 http://www.azerothcore.org/
** Conan513 https://github.com/conan513
** Made into a module by Micrah https://github.com/milestorme/
*/

#include "Chat.h"
#include "ChannelMgr.h"
#include "Config.h"
#include "cs_playerbots.h"
#include "DatabaseEnv.h"
#include "DBUpdater.h"
#include "Log.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSocket.h"

#include "ExternalEventHelper.h"
#include "Playerbots.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotTextMgr.h"
#include "RandomPlayerbotMgr.h"
#include "RandomItemManager.h"
#include "RandomPlayerbotBracketMgr.h"
#include "TravelMgr.h"

#include "Authentication/AuthCrypt.h"
#include "CharacterHandler.h"

#include <algorithm>
#include <cctype>
#include <vector>
#include <future>
#include <unordered_set>
#include <random>
#include <thread>

namespace
{
    bool IsRealPlayer(Player* player)
    {
        if (!player)
            return false;

        PlayerbotAI* playerAI = GET_PLAYERBOT_AI(player);
        return !playerAI || playerAI->IsRealPlayer();
    }

    bool IsLfgCommand(std::string const& msg, std::string& outCommand)
    {
        size_t start = msg.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
            return false;

        outCommand = msg.substr(start);
        std::string lowered = outCommand;
        std::transform(lowered.begin(), lowered.end(), lowered.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (lowered.rfind("lfg", 0) != 0)
            return false;

        if (lowered.size() > 3 && lowered[3] != ' ')
            return false;

        return true;
    }

    void DispatchLfgToBot(Player* sender, Player* botPlayer, std::string const& command)
    {
        if (!sender || !botPlayer || sender == botPlayer)
            return;

        if (!IsRealPlayer(sender))
            return;

        PlayerbotAI* botAI = GET_PLAYERBOT_AI(botPlayer);
        if (!botAI)
            return;

        ExternalEventHelper helper(botAI->GetAiObjectContext());
        helper.ParseChatCommand(command, sender);
    }
}

class mod_playerbots : public PlayerScript
{
public:
    mod_playerbots() : PlayerScript("mod_playerbots") {}
    /*void OnLogin(Player* player) override{}*/
};

class PlayerbotsDatabaseScript : public DatabaseScript
{
public:
    PlayerbotsDatabaseScript() : DatabaseScript("PlayerbotsDatabaseScript") {}

    static bool ApplyUpdates()
    {
        if (!sConfigMgr->GetBoolDefault("Playerbots.Updates.EnableDatabases", true))
            return true;

        CharacterDatabase.PExecute("INSERT IGNORE INTO updates_include (`path`, `state`) VALUES ('$/modules/mod_playerbots/data/sql/characters/updates', 'RELEASED')");
        WorldDatabase.PExecute("INSERT IGNORE INTO updates_include (`path`, `state`) VALUES ('$/modules/mod_playerbots/data/sql/world/updates', 'RELEASED')");

        if (!DBUpdater<CharacterDatabaseConnection>::Update(CharacterDatabase))
            return false;
        if (!DBUpdater<WorldDatabaseConnection>::Update(WorldDatabase))
            return false;

        return true;
    }

    bool OnDatabasesLoading() override
    {
        return ApplyUpdates();
    }
};

class PlayerbotsWorldScript : public WorldScript
{
public:
    PlayerbotsWorldScript() : WorldScript("PlayerbotsWorldScript") {}

    void OnConfigLoad(bool reloaded) override
    {
        if (!reloaded)
        {
            uint32 oldMSTime = getMSTime();

            TC_LOG_INFO("playerbots", " ");
            TC_LOG_INFO("playerbots", "Loading Playerbots Config...");
            sPlayerbotAIConfig->Initialize();

            TC_LOG_INFO("playerbots", ">> Loaded playerbots config in %u ms", GetMSTimeDiffToNow(oldMSTime));
            TC_LOG_INFO("playerbots", " ");

            sRandomPlayerbotMgr->Reserve(sPlayerbotAIConfig->maxRandomBots);
            sRandomItemMgr->Init();

            TravelMgr::instance().LoadQuestTravelTable();

            TC_LOG_INFO("playerbots", "Playerbots enabled: %s", sPlayerbotAIConfig->enabled ? "Yes" : "No");
            TC_LOG_INFO("playerbots", "Playerbots min/max to load: %u/%u", sPlayerbotAIConfig->minRandomBots, sPlayerbotAIConfig->maxRandomBots);
            TC_LOG_INFO("playerbots", "Playerbots autologin: %s", sPlayerbotAIConfig->randomBotAutologin ? "Yes" : "No");
        }
    }
    void OnUpdate(uint32 diff) override
    {
        sBracketMgr->Update(diff);
    }
};


class PlayerbotsServerScript : public ServerScript
{
public:
    PlayerbotsServerScript() : ServerScript("PlayerbotsServerScript") {}
    void OnPacketReceive(WorldSession* sessionBot, WorldPacket& packet) override
    {
        Player* playerBot = sessionBot->GetPlayer();
            if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(playerBot))
                playerbotMgr->HandleMasterIncomingPacket(packet);
    }
};

class PlayerbotsScript : public PlayerbotScript
{
public:
    PlayerbotsScript() : PlayerbotScript("PlayerbotsScript") {}

    void OnPlayerbotCheckKillTask(Player* /*player*/, Unit* /*victim*/) override
    {
    }

    void OnPlayerbotCheckPetitionAccount(Player* player, bool& found) override
    {
        if (found && GET_PLAYERBOT_AI(player))
            found = false;
    }

    bool OnPlayerbotCheckUpdatesToSend(Player* player) override
    {
        if (PlayerbotAI* botAI = GET_PLAYERBOT_AI(player))
            return botAI->IsRealPlayer();

        return true;
    }

    void OnPlayerbotPacketSent(Player* player, WorldPacket const* packet) override
    {
        if (!player)
            return;
        
        if (PlayerbotAI* botAI = GET_PLAYERBOT_AI(player))
        {
            botAI->HandleBotOutgoingPacket(*packet);
        }
        if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(player))
        {
            playerbotMgr->HandleMasterOutgoingPacket(*packet);
        }
    }

    void OnPlayerbotUpdate(uint32 diff) override
    {
        sRandomPlayerbotMgr->UpdateAI(diff);
        sRandomPlayerbotMgr->UpdateSessions();
    }

    void OnPlayerbotUpdateSessions(Player* player) override
    {
        if (player)
            if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(player))
                playerbotMgr->UpdateSessions();
    }

    void OnPlayerbotLogout(Player* player) override
    {
        if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(player))
        {
            PlayerbotAI* botAI = GET_PLAYERBOT_AI(player);
            if (!botAI || botAI->IsRealPlayer())
            {
                playerbotMgr->LogoutAllBots();
            }
        }

        sRandomPlayerbotMgr->OnPlayerLogout(player);
    }

    void OnPlayerbotLogoutBots() override { sRandomPlayerbotMgr->LogoutAllBots(); }
};
class PlayerbotsPlayerScript : public PlayerScript
{
public:
    PlayerbotsPlayerScript() : PlayerScript("PlayerbotsPlayerScript") {}

    void OnLogin(Player* player) override
    {
        if (!player->GetSession()->IsBot())
        {
            sPlayerbotsMgr->AddPlayerbotData(player, false);
            sRandomPlayerbotMgr->OnPlayerLogin(player);
            sPlayerbotTextMgr->AddLocalePriority(player->GetSession()->GetSessionDbcLocale());

            if (sPlayerbotAIConfig->enabled || sPlayerbotAIConfig->randomBotAutologin)
            {
                std::string roundedTime =
                    std::to_string(std::ceil((sPlayerbotAIConfig->maxRandomBots * 0.11 / 60) * 10) / 10.0);
                roundedTime = roundedTime.substr(0, roundedTime.find('.') + 2);

                ChatHandler(player->GetSession()).SendSysMessage(std::string("Playerbots: bot initialization at server startup takes about '" + roundedTime + "' minutes.").c_str());
            }
        }
    }

    void OnAfterUpdate(Player* player, uint32 diff) override
    {
        if (PlayerbotAI* botAI = GET_PLAYERBOT_AI(player))
        {
            botAI->UpdateAI(diff);
        }

        if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(player))
        {
            playerbotMgr->UpdateAI(diff);
        }
    }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Player* receiver) override
    {
        if (!player || !receiver || type != CHAT_MSG_WHISPER)
            return;

        std::string command;
        if (!IsLfgCommand(msg, command))
            return;

        DispatchLfgToBot(player, receiver, command);
    }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg) override
    {
        if (!player)
            return;

        if (type != CHAT_MSG_SAY && type != CHAT_MSG_YELL)
            return;

        std::string command;
        if (!IsLfgCommand(msg, command))
            return;

        Map* map = player->GetMap();
        if (!map)
            return;

        float range = sPlayerbotAIConfig->sightDistance;
        Map::PlayerList const& players = map->GetPlayers();
        for (auto const& itr : players)
        {
            Player* target = itr.GetSource();
            if (!target || target == player)
                continue;

            if (player->GetDistance(target) > range)
                continue;

            DispatchLfgToBot(player, target, command);
        }
    }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Group* group) override
    {
        if (!player || !group)
            return;

        if (type != CHAT_MSG_PARTY && type != CHAT_MSG_RAID)
            return;

        std::string command;
        if (!IsLfgCommand(msg, command))
            return;

        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            if (!member)
                continue;

            DispatchLfgToBot(player, member, command);
        }
    }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Guild* guild) override
    {
        if (!player || !guild)
            return;

        if (type != CHAT_MSG_GUILD && type != CHAT_MSG_OFFICER)
            return;

        std::string command;
        if (!IsLfgCommand(msg, command))
            return;

        uint32 guildId = player->GetGuildId();
        if (!guildId)
            return;

        auto const& players = ObjectAccessor::GetPlayers();
        for (auto const& itr : players)
        {
            Player* member = itr.second;
            if (!member || member->GetGuildId() != guildId)
                continue;

            DispatchLfgToBot(player, member, command);
        }
    }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Channel* /*channel*/) override
    {
        if (!player || type != CHAT_MSG_CHANNEL)
            return;

        std::string command;
        if (!IsLfgCommand(msg, command))
            return;

        uint32 zoneId = player->GetZoneId();
        auto const& players = ObjectAccessor::GetPlayers();
        for (auto const& itr : players)
        {
            Player* member = itr.second;
            if (!member || member->GetZoneId() != zoneId)
                continue;

            DispatchLfgToBot(player, member, command);
        }
    }
};
void Addmod_playerbotsScripts()
{
    new mod_playerbots();

    new PlayerbotsDatabaseScript();
    new PlayerbotsServerScript();
    new PlayerbotsWorldScript();
    new PlayerbotsScript();
    new PlayerbotsPlayerScript();

    AddSC_playerbots_commandscript();
}

void AddSC_mod_playerbots()
{
    Addmod_playerbotsScripts();
}
