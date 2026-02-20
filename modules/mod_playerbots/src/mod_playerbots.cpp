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
#include "Chat.h"
#include "ChannelMgr.h"
#include "Config.h"
#include "cs_playerbots.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Creature.h"
#include "Player.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSocket.h"

#include "ExternalEventHelper.h"
#include "Playerbots.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotTextMgr.h"
#include "RandomPlayerbotMgr.h"
#include "WorldThread/PlayerbotWorldThreadProcessor.h"
#include "RandomItemManager.h"
#include "TravelMgr.h"
#include "PlayerbotLLMInterface.h"
#include "PlayerbotSecurity.h"

#include "Authentication/AuthCrypt.h"
#include "CharacterHandler.h"
#include "Mail.h"
#include "ObjectAccessor.h"

#include <algorithm>
#include <cctype>
#include <vector>
#include <future>
#include <unordered_set>
#include <unordered_map>
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

    bool StartsWithCommandToken(std::string const& text, std::string const& token)
    {
        return text.rfind(token, 0) == 0 && (text.size() == token.size() || text[token.size()] == ' ');
    }

    bool ParsePlayerbotCommand(std::string const& msg, std::string& outCommand, std::string& outFilter)
    {
        outFilter.clear();

        size_t start = msg.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
            return false;

        std::string trimmed = msg.substr(start);

        // Optional leading filter: "@something ..." (applies only to broadcast targeting; not part of the actual command).
        if (!trimmed.empty() && trimmed[0] == '@')
        {
            size_t sp = trimmed.find(' ');
            if (sp != std::string::npos)
            {
                outFilter = trimmed.substr(0, sp);
                trimmed = trimmed.substr(sp + 1);
                start = trimmed.find_first_not_of(" \t\r\n");
                if (start == std::string::npos)
                    return false;
                trimmed = trimmed.substr(start);
            }
        }

        std::string lowered = trimmed;
        std::transform(lowered.begin(), lowered.end(), lowered.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        static std::string const groupSummonCommand = "group summon";
        static std::string const groupReviveCommand = "group revive";

        if (StartsWithCommandToken(lowered, groupSummonCommand))
        {
            outCommand = "summon";
            if (trimmed.size() > groupSummonCommand.size())
                outCommand += trimmed.substr(groupSummonCommand.size());
            return true;
        }

        if (StartsWithCommandToken(lowered, groupReviveCommand))
        {
            outCommand = "revive";
            if (trimmed.size() > groupReviveCommand.size())
                outCommand += trimmed.substr(groupReviveCommand.size());
            return true;
        }

        if (StartsWithCommandToken(lowered, "lfg"))
        {
            outCommand = "lfg";
            if (trimmed.size() > 3)
                outCommand += trimmed.substr(3);
            return true;
        }

        if (StartsWithCommandToken(lowered, "summon"))
        {
            outCommand = "summon";
            if (trimmed.size() > 6)
                outCommand += trimmed.substr(6);
            return true;
        }

        if (StartsWithCommandToken(lowered, "revive"))
        {
            outCommand = "revive";
            if (trimmed.size() > 6)
                outCommand += trimmed.substr(6);
            return true;
        }

        if (StartsWithCommandToken(lowered, "bank"))
        {
            outCommand = "bank";
            if (trimmed.size() > 4)
                outCommand += trimmed.substr(4);
            return true;
        }

        if (StartsWithCommandToken(lowered, "mail"))
        {
            outCommand = "mail";
            if (trimmed.size() > 4)
                outCommand += trimmed.substr(4);
            return true;
        }

        if (StartsWithCommandToken(lowered, "ah"))
        {
            outCommand = "ah";
            if (trimmed.size() > 2)
                outCommand += trimmed.substr(2);
            return true;
        }

        if (StartsWithCommandToken(lowered, "help"))
        {
            outCommand = "help";
            if (trimmed.size() > 4)
                outCommand += trimmed.substr(4);
            return true;
        }

        return false;
    }

    bool PassesSingleFilter(PlayerbotAI* botAI, Player* sender, std::string const& f)
    {
        if (!botAI || !botAI->GetBot() || !sender)
            return false;

        Player* bot = botAI->GetBot();
        bool inGroup = bot->GetGroup() != nullptr;

        if (f == "@tank")
            return botAI->IsTank(bot, inGroup);
        if (f == "@notank")
            return !botAI->IsTank(bot, inGroup);
        if (f == "@heal")
            return botAI->IsHeal(bot, inGroup);
        if (f == "@noheal")
            return !botAI->IsHeal(bot, inGroup);
        if (f == "@dps")
            return !botAI->IsTank(bot, inGroup) && !botAI->IsHeal(bot, inGroup);
        if (f == "@nodps")
            return botAI->IsTank(bot, inGroup) || botAI->IsHeal(bot, inGroup);
        if (f == "@ranged")
            return botAI->IsRanged(bot, inGroup);
        if (f == "@noranged")
            return !botAI->IsRanged(bot, inGroup);
        if (f == "@melee")
            return !botAI->IsRanged(bot, inGroup);
        if (f == "@nomelee")
            return botAI->IsRanged(bot, inGroup);
        if (f == "@guild")
            return bot->GetGuildId() != 0;
        if (f == "@noguild")
            return bot->GetGuildId() == 0;
        if (f == "@group")
            return bot->GetGroup() != nullptr;
        if (f == "@nogroup")
            return bot->GetGroup() == nullptr;
        if (f == "@mine")
            return botAI->GetMaster() == sender;

        // Class filters: "@mage", "@nomage", etc.
        auto classForToken = [](std::string const& token, uint8& outClass) -> bool
        {
            if (token == "warrior") { outClass = CLASS_WARRIOR; return true; }
            if (token == "paladin") { outClass = CLASS_PALADIN; return true; }
            if (token == "hunter")  { outClass = CLASS_HUNTER; return true; }
            if (token == "rogue")   { outClass = CLASS_ROGUE; return true; }
            if (token == "priest")  { outClass = CLASS_PRIEST; return true; }
            if (token == "deathknight" || token == "dk") { outClass = CLASS_DEATH_KNIGHT; return true; }
            if (token == "shaman")  { outClass = CLASS_SHAMAN; return true; }
            if (token == "mage")    { outClass = CLASS_MAGE; return true; }
            if (token == "warlock") { outClass = CLASS_WARLOCK; return true; }
            if (token == "druid")   { outClass = CLASS_DRUID; return true; }
            if (token == "monk")    { outClass = CLASS_MONK; return true; }
            return false;
        };

        if (f.size() > 1 && f[0] == '@')
        {
            bool negate = false;
            std::string token = f.substr(1);
            if (token.rfind("no", 0) == 0 && token.size() > 2)
            {
                negate = true;
                token = token.substr(2);
            }

            uint8 cls = 0;
            if (classForToken(token, cls))
                return negate ? (bot->GetClass() != cls) : (bot->GetClass() == cls);
        }

        // RTI filters (raid target icons): "@skull", "@star", etc.
        if (bot->GetGroup())
        {
            auto rtiIndex = [](std::string const& rti) -> int
            {
                if (rti == "@star") return 0;
                if (rti == "@circle") return 1;
                if (rti == "@diamond") return 2;
                if (rti == "@triangle") return 3;
                if (rti == "@moon") return 4;
                if (rti == "@square") return 5;
                if (rti == "@cross") return 6;
                if (rti == "@skull") return 7;
                return -1;
            };

            int idx = rtiIndex(f);
            if (idx >= 0 && idx < TARGETICONCOUNT)
            {
                ObjectGuid icon = bot->GetGroup()->GetTargetIcon(static_cast<uint8>(idx));
                if (icon.IsEmpty())
                    return false;
                return bot->GetGUID() == icon || bot->GetTarget() == icon;
            }
        }

        // Strategy filters: "@nc=strat", "@nonc=strat", "@co=strat", "@noco=strat", "@dead=strat", "@nodead=strat"
        auto parseEq = [&f](char const* pfx, std::string& out) -> bool
        {
            size_t n = std::strlen(pfx);
            if (f.rfind(pfx, 0) != 0)
                return false;
            out = f.substr(n);
            return !out.empty();
        };

        std::string strat;
        if (parseEq("@nc=", strat))
            return botAI->HasStrategy(strat, BOT_STATE_NON_COMBAT);
        if (parseEq("@nonc=", strat))
            return !botAI->HasStrategy(strat, BOT_STATE_NON_COMBAT);
        if (parseEq("@co=", strat))
            return botAI->HasStrategy(strat, BOT_STATE_COMBAT);
        if (parseEq("@noco=", strat))
            return !botAI->HasStrategy(strat, BOT_STATE_COMBAT);
        if (parseEq("@dead=", strat))
            return botAI->HasStrategy(strat, BOT_STATE_DEAD);
        if (parseEq("@nodead=", strat))
            return !botAI->HasStrategy(strat, BOT_STATE_DEAD);

        // Level filter: "@60" or "@10-20"
        if (f.size() >= 2 && f[0] == '@' && std::isdigit(static_cast<unsigned char>(f[1])))
        {
            std::string spec = f.substr(1);
            size_t dash = spec.find('-');
            if (dash != std::string::npos)
            {
                uint32 fromL = static_cast<uint32>(std::strtoul(spec.substr(0, dash).c_str(), nullptr, 10));
                uint32 toL = static_cast<uint32>(std::strtoul(spec.substr(dash + 1).c_str(), nullptr, 10));
                if (fromL && toL && fromL <= toL)
                    return bot->GetLevel() >= fromL && bot->GetLevel() <= toL;
            }
            else
            {
                uint32 level = static_cast<uint32>(std::strtoul(spec.c_str(), nullptr, 10));
                if (level)
                    return bot->GetLevel() == level;
            }
        }

        // Unknown filter: reject.
        return false;
    }

    bool PassesFilter(PlayerbotAI* botAI, Player* sender, std::string const& filterToken)
    {
        if (filterToken.empty())
            return true;

        // Allow chaining like "@tank@60@nc=rpg".
        std::string f = filterToken;
        std::transform(f.begin(), f.end(), f.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        size_t pos = 0;
        while (pos < f.size())
        {
            size_t at = f.find('@', pos);
            if (at == std::string::npos)
                break;
            size_t nextAt = f.find('@', at + 1);
            std::string part = (nextAt == std::string::npos) ? f.substr(at) : f.substr(at, nextAt - at);
            if (!PassesSingleFilter(botAI, sender, part))
                return false;
            if (nextAt == std::string::npos)
                break;
            pos = nextAt;
        }

        return true;
    }

    void DispatchCommandToBot(Player* sender, Player* botPlayer, std::string const& command, std::string const& filterToken)
    {
        if (!sender || !botPlayer || sender == botPlayer)
            return;

        if (!IsRealPlayer(sender))
            return;

        PlayerbotAI* botAI = GET_PLAYERBOT_AI(botPlayer);
        if (!botAI)
            return;

        if (!PassesFilter(botAI, sender, filterToken))
            return;

        // Security gate: most commands require invite-level relationship.
        // Align with CMaNGOS: allow a small hardcoded set of commands without invite-level security.
        // This keeps random bots from being "remote controlled" via say/yell/channel spam.
        std::string lowerCmd = command;
        std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        static std::vector<std::string> const kUnsecuredPrefixes =
        {
            "who",
            "wts",
            "sendmail",
            "invite",
            "leave",
            "join",
            "lfg",
            "guild invite",
            "guild leave"
        };

        bool unsecured = false;
        for (std::string const& pfx : kUnsecuredPrefixes)
        {
            if (lowerCmd.rfind(pfx, 0) == 0)
            {
                unsecured = true;
                break;
            }
        }

        PlayerbotSecurityLevel need = unsecured ? PlayerbotSecurityLevel::PLAYERBOT_SECURITY_TALK
                                                : PlayerbotSecurityLevel::PLAYERBOT_SECURITY_INVITE;
        if (botAI->GetSecurity() && !botAI->GetSecurity()->CheckLevelFor(need, false, sender))
            return;

        ExternalEventHelper helper(botAI->GetAiObjectContext());
        helper.ParseChatCommand(command, sender);
    }

    void ReplaceAll(std::string& s, std::string const& from, std::string const& to)
    {
        if (from.empty())
            return;
        size_t pos = 0;
        while ((pos = s.find(from, pos)) != std::string::npos)
        {
            s.replace(pos, from.size(), to);
            pos += to.size();
        }
    }

    // NOTE: this is per-process memory only (not persisted).
    std::mutex g_llmMtx;
    std::unordered_map<std::string, std::string> g_llmContext;

    struct PendingLLMReply
    {
        ObjectGuid botGuid;
        ObjectGuid playerGuid;
        std::vector<std::string> lines;
    };

    std::mutex g_llmReplyMtx;
    std::vector<PendingLLMReply> g_llmReplies;

    std::string MakeLLMKey(ObjectGuid botGuid, bool globalContext, uint32 chatType, std::string const& senderName)
    {
        std::ostringstream out;
        out << botGuid.GetCounter() << ":";
        if (globalContext)
        {
            out << "global";
        }
        else
        {
            // Per-channel/per-sender context.
            out << chatType << ":" << senderName;
        }
        return out.str();
    }

    bool TryHandleLLMChat(Player* sender, Player* receiverBot, uint32 chatType, std::string const& msg)
    {
        if (!sender || !receiverBot || sender == receiverBot)
            return false;

        if (!IsRealPlayer(sender))
            return false;

        PlayerbotAI* botAI = GET_PLAYERBOT_AI(receiverBot);
        if (!botAI || botAI->IsRealPlayer())
            return false;

        if (!sPlayerbotAIConfig->llmEnabled)
            return false;

        if (!botAI->HasStrategy("ai chat", BOT_STATE_NON_COMBAT))
            return false;

        if (!sPlayerbotAIConfig->llmApiEndpoint.size())
            return false;

        if (sPlayerbotAIConfig->llmBlockedReplyChannels.find(chatType) != sPlayerbotAIConfig->llmBlockedReplyChannels.end())
            return false;

        // Don't allow storms.
        if (!PlayerbotLLMInterface::TryAcquireGenerationSlot(sPlayerbotAIConfig->llmMaxSimultaniousGenerations))
            return false;

        PlayerbotLLMInterface::UrlParts url = PlayerbotLLMInterface::ParseUrl(sPlayerbotAIConfig->llmApiEndpoint);
        if (!url.valid)
        {
            PlayerbotLLMInterface::ReleaseGenerationSlot();
            return false;
        }

        ObjectGuid botGuid = receiverBot->GetGUID();
        std::string senderName = sender->GetName();
        std::string botName = receiverBot->GetName();

        std::string key = MakeLLMKey(botGuid, sPlayerbotAIConfig->llmGlobalContext, chatType, senderName);

        // Prepare prompt JSON in a worker thread.
        std::string apiKey = sPlayerbotAIConfig->llmApiKey;
        std::string apiJson = sPlayerbotAIConfig->llmApiJson;
        if (apiJson.empty())
        {
            // Generic OpenAI-style template (works for many OpenAI-compatible gateways).
            apiJson = R"({"model":"gpt-4o-mini","messages":[{"role":"system","content":"<pre prompt> <context>"},{"role":"user","content":"<prompt>"}],"max_tokens":60})";
        }

        std::string prePrompt = sPlayerbotAIConfig->llmPrePrompt;
        if (prePrompt.empty())
        {
            prePrompt = "You are a roleplaying character in World of Warcraft: <expansion name>. Your name is <bot name>. Respond in character and keep it short.";
        }

        std::string prompt = sPlayerbotAIConfig->llmPrompt;
        std::string postPrompt = sPlayerbotAIConfig->llmPostPrompt;

        // Capture only primitive ids for thread safety.
        ObjectGuid senderGuid = sender->GetGUID();

        std::string msgCopy = msg;
        std::thread([botGuid, senderGuid, senderName, botName, chatType, key, msgCopy, url, apiKey, apiJson, prePrompt, prompt, postPrompt]()
        {
            std::string context;
            {
                std::lock_guard<std::mutex> lock(g_llmMtx);
                auto& ref = g_llmContext[key];
                context = ref;
            }

            // Build simple context in "<name>: text" format.
            if (!context.empty())
                context += " ";
            context += senderName + ":" + msgCopy;
            PlayerbotLLMInterface::LimitContext(context, sPlayerbotAIConfig->llmContextLength);

            std::string filled = apiJson;
            std::string filledPre = prePrompt;
            ReplaceAll(filledPre, "<expansion name>", sPlayerbotAIConfig->llmExpansionName);
            ReplaceAll(filledPre, "<bot name>", botName);

            std::string filledPrompt = prompt;
            ReplaceAll(filledPrompt, "<receiver name>", botName);
            ReplaceAll(filledPrompt, "<initial message>", msgCopy);

            std::string filledPost = postPrompt;
            ReplaceAll(filledPost, "<bot name>", botName);
            ReplaceAll(filledPost, "<sender name>", senderName);

            ReplaceAll(filled, "<pre prompt>", filledPre);
            ReplaceAll(filled, "<context>", context);
            ReplaceAll(filled, "<prompt>", filledPrompt);
            ReplaceAll(filled, "<post prompt>", filledPost);

            std::string responseBody;
            bool ok = PlayerbotLLMInterface::HttpPostJson(url, apiKey, filled, sPlayerbotAIConfig->llmGenerationTimeout, responseBody);
            std::vector<std::string> lines;
            if (ok)
            {
                std::string startPat = sPlayerbotAIConfig->llmResponseStartPattern;
                std::string endPat = sPlayerbotAIConfig->llmResponseEndPattern;
                std::string delPat = sPlayerbotAIConfig->llmResponseDeletePattern;
                std::string splitPat = sPlayerbotAIConfig->llmResponseSplitPattern;

                // CMaNGOS parity: allow patterns to reference runtime values.
                ReplaceAll(startPat, "<sender name>", senderName);
                ReplaceAll(endPat, "<sender name>", senderName);
                ReplaceAll(delPat, "<sender name>", senderName);
                ReplaceAll(splitPat, "<sender name>", senderName);

                lines = PlayerbotLLMInterface::ParseResponse(responseBody,
                    startPat, endPat, delPat, splitPat);
            }

            // Update context with bot response (best-effort).
            if (!lines.empty())
            {
                std::string botReply;
                for (std::string const& l : lines)
                {
                    if (!botReply.empty())
                        botReply += " ";
                    botReply += l;
                }
                if (!context.empty())
                    context += " ";
                context += botName + ":" + botReply;
                PlayerbotLLMInterface::LimitContext(context, sPlayerbotAIConfig->llmContextLength);
                std::lock_guard<std::mutex> lock(g_llmMtx);
                g_llmContext[key] = context;
            }

            if (!lines.empty())
            {
                std::lock_guard<std::mutex> lock(g_llmReplyMtx);
                g_llmReplies.push_back(PendingLLMReply{ botGuid, senderGuid, lines });
            }

            PlayerbotLLMInterface::ReleaseGenerationSlot();
        }).detach();
        return true;
    }
}

class mod_playerbots : public PlayerScript
{
public:
    mod_playerbots() : PlayerScript("mod_playerbots") {}
    /*void OnLogin(Player* player) override{}*/
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
        PlayerbotWorldThreadProcessor::instance().Update(diff);

        // Dispatch any completed LLM replies on the world thread.
        std::vector<PendingLLMReply> pending;
        {
            std::lock_guard<std::mutex> lock(g_llmReplyMtx);
            pending.swap(g_llmReplies);
        }

        for (PendingLLMReply const& r : pending)
        {
            Player* bot = ObjectAccessor::FindPlayer(r.botGuid);
            Player* player = ObjectAccessor::FindPlayer(r.playerGuid);
            if (!bot || !player)
                continue;

            uint32 delay = 250;
            for (std::string const& l : r.lines)
            {
                std::string line = l;
                bot->AddDelayedEvent(delay, [botGuid = r.botGuid, playerGuid = r.playerGuid, line]()
                {
                    Player* b = ObjectAccessor::FindPlayer(botGuid);
                    Player* p = ObjectAccessor::FindPlayer(playerGuid);
                    if (!b || !p)
                        return;
                    b->Whisper(line, b->GetTeamId() == TEAM_ALLIANCE ? LANG_COMMON : LANG_ORCISH, p);
                });
                delay += 500;
            }
        }
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
        if (!player || !packet || !player->GetSession() || player->IsDuringRemoveFromWorld())
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
        if (!player)
            return;

        if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(player))
        {
            PlayerbotAI* botAI = GET_PLAYERBOT_AI(player);
            if (!botAI || botAI->IsRealPlayer())
            {
                playerbotMgr->LogoutAllBots();
            }
        }

        sRandomPlayerbotMgr->OnPlayerLogout(player);
        sPlayerbotsMgr->RemovePlayerBotData(player->GetGUID(), true);
        sPlayerbotsMgr->RemovePlayerBotData(player->GetGUID(), false);
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
        if (!player || !player->GetSession())
            return;

        if (player->GetSession()->IsBot())
        {
            if (PlayerbotAI* botAI = GET_PLAYERBOT_AI(player))
            {
                uint32 const targetDiff = sRandomPlayerbotMgr->HasPlayers() ? sPlayerbotAIConfig->diffWithPlayer : sPlayerbotAIConfig->diffEmpty;
                bool const minimal = sWorld->GetUpdateTime() > targetDiff;
                botAI->UpdateAI(diff, minimal);
            }
        }
        else
        {
            if (PlayerbotMgr* playerbotMgr = GET_PLAYERBOT_MGR(player))
                playerbotMgr->UpdateAI(diff);
        }
    }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Player* receiver) override
    {
        if (!player || !receiver || type != CHAT_MSG_WHISPER)
            return;

        std::string command;
        std::string filter;
        if (ParsePlayerbotCommand(msg, command, filter))
        {
            DispatchCommandToBot(player, receiver, command, filter);
            return;
        }

        // Non-command whisper: optional LLM chat.
        TryHandleLLMChat(player, receiver, type, msg);
    }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg) override
    {
        if (!player)
            return;

        if (type != CHAT_MSG_SAY && type != CHAT_MSG_YELL)
            return;

        std::string command;
        std::string filter;
        if (!ParsePlayerbotCommand(msg, command, filter))
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

            DispatchCommandToBot(player, target, command, filter);
        }
    }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Group* group) override
    {
        if (!player || !group)
            return;

        if (type != CHAT_MSG_PARTY && type != CHAT_MSG_RAID)
            return;

        std::string command;
        std::string filter;
        if (!ParsePlayerbotCommand(msg, command, filter))
            return;

        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            if (!member)
                continue;

            DispatchCommandToBot(player, member, command, filter);
        }
    }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Guild* guild) override
    {
        if (!player || !guild)
            return;

        if (type != CHAT_MSG_GUILD && type != CHAT_MSG_OFFICER)
            return;

        std::string command;
        std::string filter;
        if (!ParsePlayerbotCommand(msg, command, filter))
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

            DispatchCommandToBot(player, member, command, filter);
        }
    }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Channel* /*channel*/) override
    {
        if (!player || type != CHAT_MSG_CHANNEL)
            return;

        std::string command;
        std::string filter;
        if (!ParsePlayerbotCommand(msg, command, filter))
            return;

        uint32 zoneId = player->GetZoneId();
        auto const& players = ObjectAccessor::GetPlayers();
        for (auto const& itr : players)
        {
            Player* member = itr.second;
            if (!member || member->GetZoneId() != zoneId)
                continue;

            DispatchCommandToBot(player, member, command, filter);
        }
    }
};
void Addmod_playerbotsScripts()
{
    new mod_playerbots();
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
