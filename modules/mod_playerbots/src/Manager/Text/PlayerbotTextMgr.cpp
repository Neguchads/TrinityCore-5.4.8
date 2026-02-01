/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#include "PlayerbotTextMgr.h"

#include "DatabaseEnv.h"
#include "Log.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "World.h"

void PlayerbotTextMgr::replaceAll(std::string& str, std::string const& from, std::string const& to)
{
    if (from.empty())
        return;

    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void PlayerbotTextMgr::LoadBotTexts()
{
    TC_LOG_INFO("playerbots", "Loading playerbots texts...");

    uint32 count = 0;
    if (QueryResult result = WorldDatabase.PQuery("SELECT `name`, `text`, `say_type`, `reply_type`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8` FROM `ai_playerbot_texts`"))
    {
        if (!result->GetRowCount())
        {
            TC_LOG_WARN("playerbots", "No playerbots texts found in ai_playerbot_texts.");
            return;
        }

        do
        {
            std::map<uint32, std::string> text;
            Field* fields = result->Fetch();
            uint32 fieldCount = result->GetFieldCount();
            auto GetStringSafe = [](Field& field) -> std::string
            {
                return field.IsNull() ? std::string() : field.GetString();
            };

            if (fieldCount < 4)
            {
                TC_LOG_WARN("playerbots", "Skipping bot text row with %u columns (expected at least 4)", fieldCount);
                continue;
            }

            std::string name = GetStringSafe(fields[0]);
            if (name.empty())
                continue;

            text[0] = GetStringSafe(fields[1]);
            uint8 sayType = fields[2].GetUInt8();
            uint8 replyType = fields[3].GetUInt8();
            for (uint8 i = 1; i < MAX_LOCALES; ++i)
            {
                uint32 index = i + 3;
                if (index >= fieldCount)
                    break;
                text[i] = GetStringSafe(fields[index]);
            }

            botTexts[name].push_back(BotTextEntry(name, text, sayType, replyType));
            ++count;
        } while (result->NextRow());
    }

    TC_LOG_INFO("playerbots", "%u playerbots texts loaded", count);
}

void PlayerbotTextMgr::LoadBotTextChance()
{
    if (!botTextChance.empty())
        return;

    QueryResult results = WorldDatabase.PQuery("SELECT name, probability FROM ai_playerbot_texts_chance");
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            std::string name = fields[0].IsNull() ? std::string() : fields[0].GetString();
            if (name.empty())
                continue;
            uint32 probability = fields[1].GetUInt32();

            botTextChance[name] = probability;
        } while (results->NextRow());
    }
}

std::string PlayerbotTextMgr::GetBotText(std::string name)
{
    if (botTexts.empty())
    {
        TC_LOG_ERROR("playerbots", "Can't get bot text %s! No bots texts loaded!", name.c_str());
        return "";
    }

    if (botTexts[name].empty())
    {
        TC_LOG_ERROR("playerbots", "Can't get bot text %s! No bots texts for this name!", name.c_str());
        return "";
    }

    std::vector<BotTextEntry>& list = botTexts[name];
    BotTextEntry textEntry = list[urand(0, list.size() - 1)];
    return !textEntry.m_text[GetLocalePriority()].empty() ? textEntry.m_text[GetLocalePriority()] : textEntry.m_text[0];
}

std::string PlayerbotTextMgr::GetBotText(std::string name, std::map<std::string, std::string> placeholders)
{
    std::string botText = GetBotText(name);
    if (botText.empty())
        return "";

    for (auto const& placeholder : placeholders)
        replaceAll(botText, placeholder.first, placeholder.second);

    return botText;
}

std::string PlayerbotTextMgr::GetBotTextOrDefault(std::string name, std::string defaultText,
    std::map<std::string, std::string> placeholders)
{
    std::string botText = GetBotText(name, placeholders);
    if (botText.empty())
    {
        for (auto const& placeholder : placeholders)
            replaceAll(defaultText, placeholder.first, placeholder.second);

        return defaultText;
    }

    return botText;
}

std::string PlayerbotTextMgr::GetBotText(ChatReplyType replyType, std::map<std::string, std::string> placeholders)
{
    if (botTexts.empty())
    {
        TC_LOG_ERROR("playerbots", "Can't get bot text reply %u! No bots texts loaded!", replyType);
        return "";
    }

    if (botTexts["reply"].empty())
    {
        TC_LOG_ERROR("playerbots", "Can't get bot text reply %u! No bots texts replies!", replyType);
        return "";
    }

    std::vector<BotTextEntry>& list = botTexts["reply"];
    std::vector<BotTextEntry> proper_list;
    for (auto const& text : list)
    {
        if (text.m_replyType == replyType)
            proper_list.push_back(text);
    }

    if (proper_list.empty())
        return "";

    BotTextEntry textEntry = proper_list[urand(0, proper_list.size() - 1)];
    std::string botText =
        !textEntry.m_text[GetLocalePriority()].empty() ? textEntry.m_text[GetLocalePriority()] : textEntry.m_text[0];
    for (auto const& placeholder : placeholders)
        replaceAll(botText, placeholder.first, placeholder.second);

    return botText;
}

std::string PlayerbotTextMgr::GetBotText(ChatReplyType replyType, std::string name)
{
    std::map<std::string, std::string> placeholders;
    placeholders["%s"] = name;

    return GetBotText(replyType, placeholders);
}

bool PlayerbotTextMgr::rollTextChance(std::string text)
{
    if (!botTextChance[text])
        return true;

    return urand(0, 100) < botTextChance[text];
}

bool PlayerbotTextMgr::GetBotText(std::string name, std::string& text)
{
    if (!rollTextChance(name))
        return false;

    text = GetBotText(name);
    return !text.empty();
}

bool PlayerbotTextMgr::GetBotText(std::string name, std::string& text, std::map<std::string, std::string> placeholders)
{
    if (!rollTextChance(name))
        return false;

    text = GetBotText(name, placeholders);
    return !text.empty();
}

void PlayerbotTextMgr::AddLocalePriority(uint32 locale)
{
    if (locale >= MAX_LOCALES)
    {
        TC_LOG_WARN("playerbots", "Ignoring locale %u for bot texts because it exceeds MAX_LOCALES (%u)",
            locale, MAX_LOCALES - 1);
        return;
    }

    botTextLocalePriority[locale]++;
}

uint32 PlayerbotTextMgr::GetLocalePriority()
{
    if (sPlayerbotAIConfig->botTextLocaleWeightsEnabled)
    {
        uint32 totalWeight = 0;
        for (uint8 i = 0; i < MAX_LOCALES; ++i)
            totalWeight += sPlayerbotAIConfig->botTextLocaleWeights[i];

        if (totalWeight > 0)
        {
            uint32 roll = urand(1, totalWeight);
            uint32 current = 0;
            for (uint8 i = 0; i < MAX_LOCALES; ++i)
            {
                current += sPlayerbotAIConfig->botTextLocaleWeights[i];
                if (roll <= current)
                    return i;
            }
        }
    }

    if (!sWorld->GetActiveSessionCount())
    {
        ResetLocalePriority();
        return 0;
    }

    uint32 topLocale = 0;
    for (uint8 i = 0; i < MAX_LOCALES; ++i)
    {
        if (botTextLocalePriority[i] > botTextLocalePriority[topLocale])
            topLocale = i;
    }

    return topLocale;
}

void PlayerbotTextMgr::ResetLocalePriority()
{
    for (uint8 i = 0; i < MAX_LOCALES; ++i)
        botTextLocalePriority[i] = 0;
}
