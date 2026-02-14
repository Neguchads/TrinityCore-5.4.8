#include "GuildBankActions.h"

#include <cstdlib>
#include <sstream>
#include <vector>

#include "Bag.h"
#include "Chat.h"
#include "GameObject.h"
#include "Guild.h"
#include "Item.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "SharedDefines.h"

namespace
{
    void WhisperTo(Player* bot, Player* owner, std::string const& msg)
    {
        if (!bot || !owner || msg.empty())
            return;
        bot->Whisper(msg, bot->GetTeamId() == TEAM_ALLIANCE ? LANG_COMMON : LANG_ORCISH, owner);
    }

    uint32 ParseItemId(std::string const& s)
    {
        if (s.empty())
            return 0;
        return static_cast<uint32>(std::strtoul(s.c_str(), nullptr, 10));
    }

    std::vector<Item*> GetBagItems(Player* bot)
    {
        std::vector<Item*> items;
        if (!bot)
            return items;

        // Backpack.
        for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
        {
            if (Item* it = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                items.push_back(it);
        }

        // Bags.
        for (uint8 bagSlot = INVENTORY_SLOT_BAG_START; bagSlot < INVENTORY_SLOT_BAG_END; ++bagSlot)
        {
            Item* bagItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bagSlot);
            Bag* bag = bagItem ? bagItem->ToBag() : nullptr;
            if (!bag)
                continue;

            for (uint32 i = 0; i < bag->GetBagSize(); ++i)
            {
                if (Item* it = bag->GetItemByPos(i))
                    items.push_back(it);
            }
        }

        return items;
    }

    GameObject* FindNearbyGuildBank(PlayerbotAI* ai, Player* bot)
    {
        if (!ai || !bot)
            return nullptr;

        AiObjectContext* context = ai->GetAiObjectContext();
        GuidVector gos = AI_VALUE(GuidVector, "nearest game objects no los");
        for (ObjectGuid const& g : gos)
        {
            if (GameObject* go = ai->GetGameObject(g))
            {
                if (bot->GetGameObjectIfCanInteractWith(go->GetGUID(), GAMEOBJECT_TYPE_GUILD_BANK))
                    return go;
            }
        }

        return nullptr;
    }
}

bool GuildBankCommandAction::Execute(Event event)
{
    Player* bot = botAI->GetBot();
    Player* owner = event.getOwner();
    if (!bot || !owner)
        return false;

    std::istringstream in(event.getParam());
    std::string arg;
    in >> arg;

    if (arg.empty())
    {
        WhisperTo(bot, owner, "gb <itemId|all>");
        return true;
    }

    Guild* guild = bot->GetGuild();
    if (!guild || !bot->GetGuildId() || bot->GetGuildId() != owner->GetGuildId())
    {
        WhisperTo(bot, owner, "gb: I'm not in your guild");
        return true;
    }

    GameObject* bank = FindNearbyGuildBank(botAI, bot);
    if (!bank)
    {
        WhisperTo(bot, owner, "gb: no guild bank nearby");
        return true;
    }

    uint32 itemEntry = 0;
    bool all = (arg == "all");
    if (!all)
    {
        itemEntry = ParseItemId(arg);
        if (!itemEntry)
        {
            WhisperTo(bot, owner, "gb: invalid itemId");
            return true;
        }
    }

    uint32 moved = 0;
    std::vector<Item*> items = GetBagItems(bot);
    for (Item* item : items)
    {
        if (!item)
            continue;
        if (!all && item->GetEntry() != itemEntry)
            continue;

        // Remember if the item is still owned/located after the move attempt.
        ObjectGuid itemGuid = item->GetGUID();
        uint8 bag = item->GetBagSlot();
        uint8 slot = item->GetSlot();
        uint32 count = item->GetCount();

        guild->SwapItemsWithInventory(bot, /*toChar=*/false, /*tabId=*/0, /*slotId=*/NULL_SLOT, bag, slot, count);

        // Check whether it left our inventory.
        if (!bot->GetItemByGuid(itemGuid))
            ++moved;

        if (!all)
            break;
    }

    std::ostringstream out;
    out << "gb: deposited " << moved;
    WhisperTo(bot, owner, out.str());
    return true;
}
