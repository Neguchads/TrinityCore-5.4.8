#include "GuildValues.h"

#include "Bag.h"
#include "DatabaseEnv.h"
#include "DBCEnums.h"
#include "DBCStores.h"
#include "Item.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "World.h"

namespace
{
uint32 constexpr GUILD_CHARTER_ITEM = 5863;
uint32 constexpr GUILD_TABARD_ITEM = 5976;

Item* FindFirstItemByEntry(Player* bot, uint32 entry)
{
    if (!bot || !entry)
        return nullptr;

    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
        if (Item* it = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
            if (it->GetEntry() == entry)
                return it;

    for (uint8 bagSlot = INVENTORY_SLOT_BAG_START; bagSlot < INVENTORY_SLOT_BAG_END; ++bagSlot)
    {
        Item* bagItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bagSlot);
        Bag* bag = bagItem ? bagItem->ToBag() : nullptr;
        if (!bag)
            continue;

        for (uint32 i = 0; i < bag->GetBagSize(); ++i)
            if (Item* it = bag->GetItemByPos(i))
                if (it->GetEntry() == entry)
                    return it;
    }

    return nullptr;
}

bool IsInCapitalCity(Player* bot)
{
    if (!bot)
        return false;

    AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(bot->GetAreaId());
    if (areaEntry)
    {
        if (areaEntry->ParentAreaID)
            areaEntry = sAreaTableStore.LookupEntry(areaEntry->ParentAreaID);
        if (areaEntry && (areaEntry->Flags & AREA_FLAG_CAPITAL))
            return true;
    }
    return false;
}
}

uint8 PetitionSignsValue::Calculate()
{
    Player* bot = botAI->GetBot();
    if (!bot)
        return 0;

    if (bot->GetGuildId())
        return 0;

    Item* petition = FindFirstItemByEntry(bot, GUILD_CHARTER_ITEM);
    if (!petition)
        return 0;

    QueryResult r = CharacterDatabase.PQuery(
        "SELECT playerguid FROM petition_sign WHERE petitionguid = '%u'",
        petition->GetGUID().GetCounter());
    return r ? uint8(r->GetRowCount()) : 0;
}

bool CanHandInPetitionValue::Calculate()
{
    Player* bot = botAI->GetBot();
    if (!bot)
        return false;

    if (bot->GetGuildId())
        return false;

    if (!FindFirstItemByEntry(bot, GUILD_CHARTER_ITEM))
        return false;

    uint32 required = sWorld->getIntConfig(CONFIG_MIN_PETITION_SIGNS);
    return AI_VALUE(uint8, "petition signs") >= required;
}

bool CanBuyTabardValue::Calculate()
{
    Player* bot = botAI->GetBot();
    if (!bot)
        return false;

    if (!bot->GetGuildId())
        return false;

    if (!IsInCapitalCity(bot))
        return false;

    if (FindFirstItemByEntry(bot, GUILD_TABARD_ITEM))
        return false;

    return true;
}
