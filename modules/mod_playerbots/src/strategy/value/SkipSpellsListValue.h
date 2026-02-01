#ifndef _PLAYERBOT_SKIP_SPELLS_LIST_VALUE_H
#define _PLAYERBOT_SKIP_SPELLS_LIST_VALUE_H

#include <set>

#include "Value.h"

class PlayerbotAI;

class SkipSpellsListValue : public ManualSetValue<std::set<uint32>>
{
public:
    SkipSpellsListValue(PlayerbotAI* botAI, std::string const name = "skip spells list")
        : ManualSetValue<std::set<uint32>>(botAI, std::set<uint32>(), name)
    {
    }
};

#endif
