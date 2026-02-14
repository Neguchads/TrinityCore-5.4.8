#include "HelpActions.h"

#include <sstream>

#include "Chat.h"
#include "ChatHelper.h"
#include "PlayerbotHelpMgr.h"
#include "Player.h"
#include "PlayerbotAI.h"

namespace
{
    void WhisperTo(Player* bot, Player* owner, std::string const& msg)
    {
        if (!bot || !owner || msg.empty())
            return;
        bot->Whisper(msg, bot->GetTeamId() == TEAM_ALLIANCE ? LANG_COMMON : LANG_ORCISH, owner);
    }
}

bool HelpCommandAction::Execute(Event event)
{
    Player* bot = botAI->GetBot();
    Player* owner = event.getOwner();
    if (!bot || !owner)
        return false;

    std::string param = event.getParam();
    std::string helpTopic;

    // Clickable link from ChatHelper::formatValue("help", ...)
    if (param.find("Hvalue:help") != std::string::npos)
        helpTopic = ChatHelper::parseValue("help", param);
    else if (param.find("[h:") != std::string::npos)
    {
        // Legacy help link: [h:topic|label]
        if (param.size() >= 4)
            helpTopic = param.substr(3, param.size() - 4);
    }

    if (param.empty())
        helpTopic = "help:main";

    std::string helpText = sPlayerbotHelpMgr->GetBotText(helpTopic);

    if (helpText.empty() && !param.empty())
    {
        std::vector<std::string> list = sPlayerbotHelpMgr->FindBotText(param);
        if (list.size() == 1)
            helpText = sPlayerbotHelpMgr->GetBotText(list.front());
        else if (!list.empty())
        {
            std::sort(list.begin(), list.end());
            helpText = sPlayerbotHelpMgr->GetBotText("help:search");

            std::string topics = PlayerbotHelpMgr::makeList(list, "[h:<part>|<part>]");
            if (!helpText.empty())
                PlayerbotHelpMgr::replace(helpText, "<found>", topics);
            else
                helpText = topics;

            PlayerbotHelpMgr::FormatHelpTopic(helpText);
        }
    }

    if (!helpText.empty())
    {
        std::istringstream iss(helpText);
        std::string line;
        while (std::getline(iss, line))
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            if (!line.empty())
                WhisperTo(bot, owner, line);
        }
        return true;
    }

    // Fallback when help table isn't present/loaded.
    WhisperTo(bot, owner, "Commands: help | bank | gb | mail | sendmail | ah | lfg | summon | revive | maintenance");
    WhisperTo(bot, owner, "Commands: reset | reset ai | reset ai soft | save ai | load ai | list ai");
    WhisperTo(bot, owner, "Tip: prefix commands with filters like '@tank@60 bank deposit all' (help filters)");
    return true;
}
