#ifndef _PLAYERBOT_DB_STORE_H
#define _PLAYERBOT_DB_STORE_H

#include "Common.h"

#include <vector>
#include <string>

class PlayerbotAI;

// Ported from CMaNGOS playerbots:
// Persists bot strategies and AI context values in the characters DB.
// Table: `playerbots_db_store` (see module SQL updates).
class PlayerbotDbStore
{
public:
    static PlayerbotDbStore* instance();

    void Save(PlayerbotAI* ai, std::string const& preset = "");
    void Load(PlayerbotAI* ai, std::string const& preset = "");
    void Reset(PlayerbotAI* ai, std::string const& preset = "");

private:
    PlayerbotDbStore() = default;
    std::string FormatStrategies(std::vector<std::string> const& strategies) const;
    void SaveValue(uint32 guidLow, std::string const& preset, std::string const& key, std::string const& value) const;
};

#define sPlayerbotDbStore PlayerbotDbStore::instance()

#endif
