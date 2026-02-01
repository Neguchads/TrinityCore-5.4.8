#ifndef _PLAYERBOT_PLAYERBOTTEXTMGR_H
#define _PLAYERBOT_PLAYERBOTTEXTMGR_H

#include <map>
#include <vector>

#include "Common.h"

#define BOT_TEXT1(name) sPlayerbotTextMgr->GetBotText(name)
#define BOT_TEXT2(name, replace) sPlayerbotTextMgr->GetBotText(name, replace)

struct BotTextEntry
{
    BotTextEntry(std::string name, std::map<uint32, std::string> text, uint32 sayType, uint32 replyType)
        : m_name(name), m_text(text), m_sayType(sayType), m_replyType(replyType)
    {
    }

    std::string m_name;
    std::map<uint32, std::string> m_text;
    uint32 m_sayType;
    uint32 m_replyType;
};

enum ChatReplyType
{
    REPLY_NOT_UNDERSTAND,
    REPLY_GRUDGE,
    REPLY_VICTIM,
    REPLY_ATTACKER,
    REPLY_HELLO,
    REPLY_NAME,
    REPLY_ADMIN_ABUSE
};

class PlayerbotTextMgr
{
public:
    PlayerbotTextMgr()
    {
        for (uint8 i = 0; i < MAX_LOCALES; ++i)
            botTextLocalePriority[i] = 0;
    }

    static PlayerbotTextMgr* instance()
    {
        static PlayerbotTextMgr instance;
        return &instance;
    }

    std::string GetBotText(std::string name, std::map<std::string, std::string> placeholders);
    std::string GetBotText(std::string name);
    std::string GetBotText(ChatReplyType replyType, std::map<std::string, std::string> placeholders);
    std::string GetBotText(ChatReplyType replyType, std::string name);
    bool GetBotText(std::string name, std::string& text);
    bool GetBotText(std::string name, std::string& text, std::map<std::string, std::string> placeholders);
    std::string GetBotTextOrDefault(std::string name, std::string defaultText,
        std::map<std::string, std::string> placeholders);
    void LoadBotTexts();
    void LoadBotTextChance();
    static void replaceAll(std::string& str, const std::string& from, const std::string& to);
    bool rollTextChance(std::string text);

    uint32 GetLocalePriority();
    void AddLocalePriority(uint32 locale);
    void ResetLocalePriority();

private:
    std::map<std::string, std::vector<BotTextEntry>> botTexts;
    std::map<std::string, uint32> botTextChance;
    uint32 botTextLocalePriority[MAX_LOCALES];
};

#define sPlayerbotTextMgr PlayerbotTextMgr::instance()

#endif
