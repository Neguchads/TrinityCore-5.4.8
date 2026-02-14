#include "PlayerbotDbStore.h"

#include <sstream>

#include "DatabaseEnv.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"

PlayerbotDbStore* PlayerbotDbStore::instance()
{
    static PlayerbotDbStore s;
    return &s;
}

std::string PlayerbotDbStore::FormatStrategies(std::vector<std::string> const& strategies) const
{
    std::ostringstream out;
    bool first = true;
    for (std::string const& s : strategies)
    {
        if (s.empty())
            continue;

        // Stored format matches CMaNGOS: "+name,+name2"
        if (!first)
            out << ",";
        out << "+" << s;
        first = false;
    }
    return out.str();
}

void PlayerbotDbStore::SaveValue(uint32 guidLow, std::string const& preset, std::string const& key, std::string const& value) const
{
    std::string presetEsc = preset;
    std::string keyEsc = key;
    std::string valueEsc = value;
    CharacterDatabase.EscapeString(presetEsc);
    CharacterDatabase.EscapeString(keyEsc);
    CharacterDatabase.EscapeString(valueEsc);

    CharacterDatabase.PExecute(
        "INSERT INTO `playerbots_db_store` (`guid`, `preset`, `key`, `value`) VALUES (%u, '%s', '%s', '%s')",
        guidLow, presetEsc.c_str(), keyEsc.c_str(), valueEsc.c_str());
}

void PlayerbotDbStore::Reset(PlayerbotAI* ai, std::string const& preset)
{
    if (!ai || !ai->GetBot())
        return;

    uint32 guid = ai->GetBot()->GetGUID().GetCounter();
    std::string presetEsc = preset;
    CharacterDatabase.EscapeString(presetEsc);
    CharacterDatabase.PExecute(
        "DELETE FROM `playerbots_db_store` WHERE `guid` = %u AND `preset` = '%s'", guid, presetEsc.c_str());
}

void PlayerbotDbStore::Save(PlayerbotAI* ai, std::string const& preset)
{
    if (!ai || !ai->GetBot())
        return;

    uint32 guid = ai->GetBot()->GetGUID().GetCounter();

    Reset(ai, preset);

    // Save context values first.
    std::vector<std::string> data = ai->GetAiObjectContext()->Save();
    for (std::string const& row : data)
        SaveValue(guid, preset, "value", row);

    SaveValue(guid, preset, "co", FormatStrategies(ai->GetStrategies(BOT_STATE_COMBAT)));
    SaveValue(guid, preset, "nc", FormatStrategies(ai->GetStrategies(BOT_STATE_NON_COMBAT)));
    SaveValue(guid, preset, "dead", FormatStrategies(ai->GetStrategies(BOT_STATE_DEAD)));
}

void PlayerbotDbStore::Load(PlayerbotAI* ai, std::string const& preset)
{
    if (!ai || !ai->GetBot())
        return;

    uint32 guid = ai->GetBot()->GetGUID().GetCounter();
    std::string presetEsc = preset;
    CharacterDatabase.EscapeString(presetEsc);

    QueryResult results = CharacterDatabase.PQuery(
        "SELECT `key`, `value` FROM `playerbots_db_store` WHERE `guid` = %u AND `preset` = '%s'",
        guid, presetEsc.c_str());
    if (!results)
        return;

    ai->ClearStrategies(BOT_STATE_COMBAT);
    ai->ClearStrategies(BOT_STATE_NON_COMBAT);
    ai->ClearStrategies(BOT_STATE_DEAD);

    // Always keep chat handling.
    ai->ChangeStrategy("+chat", BOT_STATE_COMBAT);
    ai->ChangeStrategy("+chat", BOT_STATE_NON_COMBAT);

    std::vector<std::string> values;

    do
    {
        Field* fields = results->Fetch();
        std::string key = fields[0].IsNull() ? std::string() : fields[0].GetString();
        std::string value = fields[1].IsNull() ? std::string() : fields[1].GetString();

        if (key == "value")
            values.push_back(value);
        else if (key == "co")
            ai->ChangeStrategy(value, BOT_STATE_COMBAT);
        else if (key == "nc")
            ai->ChangeStrategy(value, BOT_STATE_NON_COMBAT);
        else if (key == "dead")
            ai->ChangeStrategy(value, BOT_STATE_DEAD);
    } while (results->NextRow());

    ai->GetAiObjectContext()->Load(values);
}
