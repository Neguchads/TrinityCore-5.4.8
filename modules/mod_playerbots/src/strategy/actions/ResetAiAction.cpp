#include "ResetAiAction.h"

#include <algorithm>
#include <cctype>

#include "DatabaseEnv.h"
#include "Event.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotDbStore.h"
#include "Playerbots.h"

namespace
{
    std::string Trim(std::string s)
    {
        auto notSpace = [](unsigned char c) { return !std::isspace(c); };
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
        s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
        return s;
    }
}

bool ResetAiAction::Execute(Event event)
{
    Player* requester = event.getOwner() ? event.getOwner() : botAI->GetMaster();
    ResetValues();
    ResetStrategies();

    if (!fullReset)
    {
        // Soft reset: reload saved state (CMaNGOS behavior).
        sPlayerbotDbStore->Load(botAI, "");
    }

    if (requester && fullReset)
        botAI->TellError(requester, "AI was reset to defaults");

    return true;
}

void ResetAiAction::ResetStrategies()
{
    botAI->ResetStrategies();

    if (fullReset && !sPlayerbotAIConfig->explicitDbStoreSave)
        sPlayerbotDbStore->Save(botAI, "");
}

void ResetAiAction::ResetValues()
{
    if (!botAI || !botAI->GetBot())
        return;

    Player* bot = botAI->GetBot();
    uint32 guid = bot->GetGUID().GetCounter();

    QueryResult results = CharacterDatabase.PQuery(
        "SELECT `value` FROM `playerbots_db_store` WHERE `guid` = %u AND `preset` = '' AND `key` = 'value'", guid);
    if (!results)
        return;

    do
    {
        Field* fields = results->Fetch();
        std::string val = fields[0].IsNull() ? std::string() : fields[0].GetString();
        size_t sep = val.find('>');
        if (sep == std::string::npos)
            continue;

        std::string name = val.substr(0, sep);
        std::string text = val.substr(sep + 1);

        UntypedValue* u = botAI->GetAiObjectContext()->GetUntypedValue(name);
        if (!u)
            continue;

        if (fullReset)
            u->Reset();
        else
            u->Load(text);

    } while (results->NextRow());

    if (fullReset)
        CharacterDatabase.PExecute("DELETE FROM `playerbots_db_store` WHERE `guid` = %u AND `key` = 'value'", guid);
}

bool ResetStratsAction::Execute(Event event)
{
    Player* requester = event.getOwner() ? event.getOwner() : botAI->GetMaster();
    ResetStrategies();

    if (requester && fullReset)
        botAI->TellError(requester, "Strategies reset to defaults");

    return true;
}

bool ResetValuesAction::Execute(Event event)
{
    Player* requester = event.getOwner() ? event.getOwner() : botAI->GetMaster();
    ResetValues();

    if (requester && fullReset)
        botAI->TellError(requester, "Settings reset to defaults");

    return true;
}

bool SaveAiAction::Execute(Event event)
{
    std::string preset = Trim(event.getParam());
    if (preset.empty())
        preset = Trim(getQualifier());

    sPlayerbotDbStore->Save(botAI, preset);

    if (Player* requester = event.getOwner() ? event.getOwner() : botAI->GetMaster())
        botAI->TellError(requester, "AI saved");

    return true;
}

bool LoadAiAction::Execute(Event event)
{
    std::string preset = Trim(event.getParam());
    if (preset.empty())
        preset = Trim(getQualifier());

    sPlayerbotDbStore->Load(botAI, preset);

    if (Player* requester = event.getOwner() ? event.getOwner() : botAI->GetMaster())
        botAI->TellError(requester, "AI loaded");

    return true;
}

bool ListAiAction::Execute(Event event)
{
    Player* requester = event.getOwner() ? event.getOwner() : botAI->GetMaster();
    if (!requester || !botAI || !botAI->GetBot())
        return true;

    uint32 guid = botAI->GetBot()->GetGUID().GetCounter();

    botAI->TellError(requester, "### PRESETS AVAILABLE ###");
    QueryResult results = CharacterDatabase.PQuery(
        "SELECT DISTINCT `preset` FROM `playerbots_db_store` WHERE `guid` = %u", guid);
    if (!results)
        return true;

    do
    {
        Field* fields = results->Fetch();
        std::string preset = fields[0].IsNull() ? std::string() : fields[0].GetString();
        botAI->TellError(requester, preset.empty() ? "(default)" : preset.c_str());
    } while (results->NextRow());

    return true;
}

bool ResetAction::Execute(Event /*event*/)
{
    botAI->Reset(true);
    return true;
}
