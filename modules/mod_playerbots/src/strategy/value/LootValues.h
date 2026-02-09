#ifndef _PLAYERBOT_LOOTVALUES_H
#define _PLAYERBOT_LOOTVALUES_H

#include "LootMgr.h"
#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

class LootTemplateAccess
{
public:
    class LootGroup;
    typedef std::vector<LootGroup> LootGroups;
    LootStoreItemList Entries;
    LootGroups Groups;
};

typedef std::unordered_multimap<uint32, int32> DropMap;

class DropMapValue : public SingleCalculatedValue<DropMap*>
{
public:
    DropMapValue(PlayerbotAI* botAI) : SingleCalculatedValue(botAI, "drop map") {}

    static LootTemplateAccess const* GetLootTemplate(ObjectGuid guid, LootType type = LOOT_CORPSE);
    DropMap* Calculate() override;
};

class ItemDropListValue : public SingleCalculatedValue<std::vector<int32>>, public Qualified
{
public:
    ItemDropListValue(PlayerbotAI* botAI) : SingleCalculatedValue(botAI, "item drop list") {}

    std::vector<int32> Calculate() override;
};

class EntryLootListValue : public SingleCalculatedValue<std::vector<uint32>>, public Qualified
{
public:
    EntryLootListValue(PlayerbotAI* botAI) : SingleCalculatedValue(botAI, "entry loot list") {}

    std::vector<uint32> Calculate() override;
};

#endif
