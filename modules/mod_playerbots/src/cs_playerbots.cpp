/*
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

#include "BattleGroundTactics.h"
#include "Chat.h"
#include "PlayerbotMgr.h"
#include "PerformanceMonitor.h"
#include "PlayerbotAIConfig.h"
#include "RandomPlayerbotMgr.h"
#include "ScriptMgr.h"
#include "DatabaseEnv.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <cstdlib>
#include <sstream>

class playerbots_commandscript : public CommandScript
{
public:
    playerbots_commandscript() : CommandScript("playerbots_commandscript") {}

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> playerbotsDebugCommandTable =
        {
            { "bg",             SEC_GAMEMASTER,             true,           &HandleDebugBGCommand },
        };

        static std::vector<ChatCommand> playerbotsCommandTable =
        {
            { "bot",            SEC_PLAYER,                false,          &HandlePlayerbotCommand },
            { "add",            SEC_PLAYER,                false,          &HandlePlayerbotCommand },
            { "strategy",       SEC_PLAYER,                false,          &HandlePlayerbotCommand },
            { "pmon",           SEC_GAMEMASTER,             true,           &HandlePerfMonCommand },
            { "rndbot",         SEC_GAMEMASTER,             true,           &HandleRandomPlayerbotCommand },
            { "debug",          SEC_GAMEMASTER,             true,           playerbotsDebugCommandTable },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "playerbots",     SEC_PLAYER,                true,           playerbotsCommandTable },
            { "playerbot",      SEC_ADMINISTRATOR,          true,           &HandlePlayerbotCommand},
            { "pmon",           SEC_GAMEMASTER,             true,           &HandlePerfMonCommand},
        };
        return commandTable;
    }

    static bool HandlePlayerbotCommand(ChatHandler* handler, char const* args)
    {
        return PlayerbotMgr::HandlePlayerbotMgrCommand(handler, args);
    }

    static bool HandlePerfMonCommand(ChatHandler* handler, char const* args)
    {
        if (!strcmp(args, "reset"))
        {
            sPerformanceMonitor->Reset();
            return true;
        }

        if (!strcmp(args, "toggle"))
        {
            sPlayerbotAIConfig->perfMonEnabled = !sPlayerbotAIConfig->perfMonEnabled;
            if (sPlayerbotAIConfig->perfMonEnabled)
                TC_LOG_INFO("playerbots", "Performance monitor enabled");
            else
                TC_LOG_INFO("playerbots", "Performance monitor disabled");
            return true;
        }

        std::string arguments = args ? args : "";
        bool tick = arguments.find("tick") != std::string::npos;
        bool stack = arguments.find("stack") != std::string::npos;
        bool map = arguments.find("map") != std::string::npos;

        sPerformanceMonitor->PrintStats(tick, stack, map);
        return true;
    }

    static bool HandleDebugBGCommand(ChatHandler* handler, char const* args)
    {
        return BGTactics::HandleConsoleCommand(handler, args);
    }

    static bool HandleRandomPlayerbotCommand(ChatHandler* handler, char const* args)
    {
        if (!handler)
            return false;
        return sRandomPlayerbotMgr->HandlePlayerbotConsoleCommand(handler, args);
    }
};

void AddSC_playerbots_commandscript() { new playerbots_commandscript(); }
