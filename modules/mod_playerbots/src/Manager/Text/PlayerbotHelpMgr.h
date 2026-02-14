#ifndef _PLAYERBOT_PLAYERBOTHELPMGR_H
#define _PLAYERBOT_PLAYERBOTHELPMGR_H

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "Common.h"

struct BotHelpEntry
{
    std::string templateText;
    std::string text;
    std::map<uint32, std::string> textLocales;
};

// Ported (non-GenerateBotHelp parts) from CMaNGOS playerbots PlayerbotHelpMgr.
// Loads help topics from world DB table `ai_playerbot_help_texts`.
class PlayerbotHelpMgr
{
public:
    static PlayerbotHelpMgr* instance()
    {
        static PlayerbotHelpMgr s;
        return &s;
    }

    static void replace(std::string& text, std::string const& what, std::string const& with);
    static std::string makeList(std::vector<std::string> const& parts, std::string partFormat = "<part>", uint32 maxLength = 254);
    static void FormatHelpTopic(std::string& text);

    void FormatHelpTopics();
    void LoadBotHelpTexts();

    std::string GetBotText(std::string const& name);
    bool GetBotText(std::string const& name, std::string& outText);
    std::vector<std::string> FindBotText(std::string const& needle);

private:
    std::unordered_map<std::string, BotHelpEntry> _botHelpText;
};

#define sPlayerbotHelpMgr PlayerbotHelpMgr::instance()

#endif

