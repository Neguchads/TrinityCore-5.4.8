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
};

#endif
