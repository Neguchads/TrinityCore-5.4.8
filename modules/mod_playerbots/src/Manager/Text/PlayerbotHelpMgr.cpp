#include "PlayerbotHelpMgr.h"

#include <algorithm>
#include <cctype>

#include "DatabaseEnv.h"
#include "Log.h"
#include "PlayerbotTextMgr.h"
#include "Playerbots.h"
#include "ChatHelper.h"

void PlayerbotHelpMgr::replace(std::string& text, std::string const& what, std::string const& with)
{
    if (what.empty())
        return;

    size_t start_pos = 0;
    while ((start_pos = text.find(what, start_pos)) != std::string::npos)
    {
        text.replace(start_pos, what.size(), with);
        start_pos += with.size();
    }
}

std::string PlayerbotHelpMgr::makeList(std::vector<std::string> const& parts, std::string partFormat, uint32 maxLength)
{
    std::string retString;
    std::string currentLine;

    for (std::string const& part : parts)
    {
        if (part.length() + currentLine.length() > maxLength && !currentLine.empty())
        {
            if (!currentLine.empty() && currentLine.back() == ' ')
                currentLine.pop_back();
            retString += currentLine + "\n";
            currentLine.clear();
        }

        std::string subPart = partFormat;
        replace(subPart, "<part>", part);
        currentLine += subPart + " ";
    }

    return retString + currentLine;
}

void PlayerbotHelpMgr::FormatHelpTopic(std::string& text)
{
    // Convert legacy [h:...] / [c:...] links into client hyperlinks used by bots.
    size_t start_pos = 0;
    while ((start_pos = text.find("[h:", start_pos)) != std::string::npos)
    {
        size_t end_pos = text.find("]", start_pos);
        if (end_pos == std::string::npos)
            break;

        std::string oldLink = text.substr(start_pos, end_pos - start_pos + 1);

        if (oldLink.find("|") != std::string::npos)
        {
            std::string topicCode = oldLink.substr(3, oldLink.find("|") - 3);
            std::string topicName = oldLink.substr(oldLink.find("|") + 1);
            if (!topicName.empty())
                topicName.pop_back();

            std::string newLink = ChatHelper::formatValue("help", topicCode, topicName);
            text.replace(start_pos, oldLink.length(), newLink);
            start_pos += newLink.length();
        }
        else
        {
            start_pos = end_pos;
        }
    }

    start_pos = 0;
    while ((start_pos = text.find("[c:", start_pos)) != std::string::npos)
    {
        size_t end_pos = text.find("]", start_pos);
        if (end_pos == std::string::npos)
            break;

        std::string oldLink = text.substr(start_pos, end_pos - start_pos + 1);

        if (oldLink.find("|") != std::string::npos)
        {
            std::string topicCode = oldLink.substr(3, oldLink.find("|") - 3);
            std::string topicName = oldLink.substr(oldLink.find("|") + 1);
            if (!topicName.empty())
                topicName.pop_back();

            std::string newLink = ChatHelper::formatValue("command", topicCode, topicName, "0000FF00");
            text.replace(start_pos, oldLink.length(), newLink);
            start_pos += newLink.length();
        }
        else
        {
            start_pos = end_pos;
        }
    }
}

void PlayerbotHelpMgr::FormatHelpTopics()
{
    for (auto& kv : _botHelpText)
    {
        BotHelpEntry& e = kv.second;
        if (e.text.empty())
            e.text = e.templateText;

        FormatHelpTopic(e.text);
        for (uint8 i = 1; i < MAX_LOCALES; ++i)
        {
            auto it = e.textLocales.find(i);
            if (it != e.textLocales.end() && !it->second.empty())
                FormatHelpTopic(it->second);
        }
    }
}

void PlayerbotHelpMgr::LoadBotHelpTexts()
{
    TC_LOG_INFO("playerbots", "Loading playerbot help texts...");

    _botHelpText.clear();

    QueryResult results = WorldDatabase.PQuery(
        "SELECT `name`, `template_text`, `text`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8` "
        "FROM `ai_playerbot_help_texts`");

    uint32 count = 0;
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            std::string name = fields[0].IsNull() ? std::string() : fields[0].GetString();
            if (name.empty())
                continue;

            BotHelpEntry entry;
            entry.templateText = fields[1].IsNull() ? std::string() : fields[1].GetString();
            entry.text = fields[2].IsNull() ? std::string() : fields[2].GetString();

            for (uint8 i = 1; i < MAX_LOCALES; ++i)
            {
                // Columns: name, template_text, text, text_loc1..text_loc8
                uint32 col = 2 + i;
                std::string loc = fields[col].IsNull() ? std::string() : fields[col].GetString();
                entry.textLocales[i] = loc;
            }

            _botHelpText[name] = entry;
            ++count;
        } while (results->NextRow());
    }

    TC_LOG_INFO("playerbots", "%u playerbot help texts loaded", count);

    FormatHelpTopics();
}

std::string PlayerbotHelpMgr::GetBotText(std::string const& name)
{
    if (_botHelpText.empty())
        return "";

    auto it = _botHelpText.find(name);
    if (it == _botHelpText.end())
        return "";

    BotHelpEntry const& e = it->second;
    int32 localePrio = (int32)sPlayerbotTextMgr->GetLocalePriority();
    if (localePrio <= 0)
        return e.text;

    auto lit = e.textLocales.find((uint32)localePrio);
    if (lit != e.textLocales.end() && !lit->second.empty())
        return lit->second;

    return e.text;
}

bool PlayerbotHelpMgr::GetBotText(std::string const& name, std::string& outText)
{
    outText = GetBotText(name);
    return !outText.empty();
}

std::vector<std::string> PlayerbotHelpMgr::FindBotText(std::string const& needle)
{
    std::vector<std::string> found;
    if (needle.empty())
        return found;

    for (auto const& kv : _botHelpText)
    {
        if (kv.first.find(needle) != std::string::npos)
            found.push_back(kv.first);
    }
    return found;
}

