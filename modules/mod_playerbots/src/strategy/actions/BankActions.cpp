#include "BankActions.h"

#include <sstream>

#include "CellImpl.h"
#include "Bag.h"
#include "Chat.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Item.h"
#include "ObjectDefines.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"

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

    std::vector<Item*> GetBankItems(Player* bot)
    {
        std::vector<Item*> items;
        if (!bot)
            return items;

        // Bank main slots.
        for (uint8 slot = BANK_SLOT_ITEM_START; slot < BANK_SLOT_ITEM_END; ++slot)
        {
            if (Item* it = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                items.push_back(it);
        }

        // Bank bags.
        for (uint8 bagSlot = BANK_SLOT_BAG_START; bagSlot < BANK_SLOT_BAG_END; ++bagSlot)
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

    bool HasNearbyBanker(Player* bot)
    {
        if (!bot)
            return false;

        std::list<Unit*> units;
        float const range = INTERACTION_DISTANCE * 2.0f;
        Trinity::AnyUnitInObjectRangeCheck u_check(bot, range);
        Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher(bot, units, u_check);
        Cell::VisitAllObjects(bot, searcher, range);

        for (Unit* u : units)
        {
            Creature* c = u ? u->ToCreature() : nullptr;
            if (!c)
                continue;
            if (bot->GetNPCIfCanInteractWith(c->GetGUID(), UNIT_NPC_FLAG_BANKER))
                return true;
        }

        return false;
    }

    bool DepositItem(Player* bot, Item* item)
    {
        if (!bot || !item)
            return false;

        ItemPosCountVec dest;
        InventoryResult res = bot->CanBankItem(NULL_BAG, NULL_SLOT, dest, item, false, true);
        if (res != EQUIP_ERR_OK)
            return false;

        uint8 bag = item->GetBagSlot();
        uint8 slot = item->GetSlot();
        bot->MoveItemFromInventory(bag, slot, true);
        bot->BankItem(dest, item, true);
        return true;
    }

    bool WithdrawItem(Player* bot, Item* item)
    {
        if (!bot || !item)
            return false;

        ItemPosCountVec dest;
        InventoryResult res = bot->CanStoreItem(NULL_BAG, NULL_SLOT, dest, item, false);
        if (res != EQUIP_ERR_OK)
            return false;

        uint8 bag = item->GetBagSlot();
        uint8 slot = item->GetSlot();
        bot->MoveItemFromInventory(bag, slot, true);
        bot->MoveItemToInventory(dest, item, true);
        return true;
    }
}

bool BankCommandAction::Execute(Event event)
{
    Player* bot = botAI->GetBot();
    Player* owner = event.getOwner();
    if (!bot || !owner)
        return false;

    if (!HasNearbyBanker(bot))
    {
        WhisperTo(bot, owner, "bank: cannot find banker nearby");
        return true;
    }

    std::istringstream in(event.getParam());
    std::string sub;
    in >> sub;
    if (sub.empty())
    {
        WhisperTo(bot, owner, "bank: list | deposit <itemId|all> | withdraw <itemId>");
        return true;
    }

    if (sub == "list")
    {
        auto bankItems = GetBankItems(bot);
        uint32 shown = 0;
        for (Item* it : bankItems)
        {
            if (!it)
                continue;
            if (shown >= 10)
                break;

            std::ostringstream line;
            line << "bank item=" << it->GetEntry() << " x" << it->GetCount();
            WhisperTo(bot, owner, line.str());
            ++shown;
        }

        std::ostringstream out;
        out << "Bank items: " << bankItems.size() << " total (showing " << shown << ")";
        WhisperTo(bot, owner, out.str());
        return true;
    }

    if (sub == "deposit")
    {
        std::string arg;
        in >> arg;
        if (arg.empty())
        {
            WhisperTo(bot, owner, "bank deposit <itemId|all>");
            return true;
        }

        uint32 moved = 0;
        auto bagItems = GetBagItems(bot);
        if (arg == "all")
        {
            for (Item* it : bagItems)
                if (DepositItem(bot, it))
                    ++moved;
        }
        else
        {
            uint32 itemId = ParseItemId(arg);
            if (!itemId)
            {
                WhisperTo(bot, owner, "bank deposit: invalid itemId");
                return true;
            }

            for (Item* it : bagItems)
            {
                if (it && it->GetEntry() == itemId && DepositItem(bot, it))
                    ++moved;
            }
        }

        std::ostringstream out;
        out << "bank deposit: moved " << moved;
        WhisperTo(bot, owner, out.str());
        return true;
    }

    if (sub == "withdraw")
    {
        std::string arg;
        in >> arg;
        if (arg.empty())
        {
            WhisperTo(bot, owner, "bank withdraw <itemId>");
            return true;
        }

        uint32 itemId = ParseItemId(arg);
        if (!itemId)
        {
            WhisperTo(bot, owner, "bank withdraw: invalid itemId");
            return true;
        }

        uint32 moved = 0;
        auto bankItems = GetBankItems(bot);
        for (Item* it : bankItems)
        {
            if (it && it->GetEntry() == itemId && WithdrawItem(bot, it))
                ++moved;
        }

        std::ostringstream out;
        out << "bank withdraw: moved " << moved;
        WhisperTo(bot, owner, out.str());
        return true;
    }

    WhisperTo(bot, owner, "bank: unknown subcommand");
    return true;
}
