/*
 * Minimal ChatHelper implementation for playerbots.
 */

#ifndef _PLAYERBOT_CHATHELPER_H
#define _PLAYERBOT_CHATHELPER_H

#include <string>

#include "Common.h"

class Quest;
class SpellInfo;
class WorldObject;
struct ItemTemplate;

class ChatHelper
{
public:
    static std::string const FormatQuest(Quest const* quest);
    static std::string const FormatItem(ItemTemplate const* proto, uint32 count = 0, uint32 total = 0);
    static std::string const FormatWorldEntry(int32 entry);
    static std::string const FormatWorldobject(WorldObject const* object);
    static std::string const FormatSpell(SpellInfo const* spellInfo);

    // CMaNGOS playerbots parity helpers (used by help/command hyperlinks).
    static std::string formatValue(std::string const& type, std::string const& code, std::string const& name,
        std::string const& color = "0000FFFF");
    static std::string parseValue(std::string const& type, std::string const& text);
};

#endif
