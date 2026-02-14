#include "AuctionActions.h"

#include <cstdlib>
#include <sstream>

#include "AuctionHouseMgr.h"
#include "Bag.h"
#include "CellImpl.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Item.h"
#include "Mail.h"
#include "ObjectAccessor.h"
#include "ObjectDefines.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "World.h"
#include "Time/GameTime.h"

namespace
{
    void WhisperTo(Player* bot, Player* owner, std::string const& msg)
    {
        if (!bot || !owner || msg.empty())
            return;
        bot->Whisper(msg, bot->GetTeamId() == TEAM_ALLIANCE ? LANG_COMMON : LANG_ORCISH, owner);
    }

    uint32 ParseU32(std::string const& s)
    {
        if (s.empty())
            return 0;
        return static_cast<uint32>(std::strtoul(s.c_str(), nullptr, 10));
    }

    uint64 ParseU64(std::string const& s)
    {
        if (s.empty())
            return 0;
        return static_cast<uint64>(std::strtoull(s.c_str(), nullptr, 10));
    }

    uint8 ChooseHouseId(Player* bot)
    {
        if (!bot)
            return AUCTIONHOUSE_NEUTRAL;
        if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
            return AUCTIONHOUSE_NEUTRAL;
        return bot->GetTeamId() == TEAM_ALLIANCE ? AUCTIONHOUSE_ALLIANCE : AUCTIONHOUSE_HORDE;
    }

    Item* FindSellItem(Player* bot, uint32 itemEntry, uint32 count)
    {
        if (!bot || !itemEntry || !count)
            return nullptr;

        // Backpack.
        for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
        {
            if (Item* it = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                if (it->GetEntry() == itemEntry && it->GetCount() >= count)
                    return it;
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
                    if (it->GetEntry() == itemEntry && it->GetCount() >= count)
                        return it;
            }
        }

        return nullptr;
    }

    bool HasNearbyAuctioneer(Player* bot)
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
            if (bot->GetNPCIfCanInteractWith(c->GetGUID(), UNIT_NPC_FLAG_AUCTIONEER))
                return true;
        }

        return false;
    }

    bool FindAccessibleAuctionHouse(Player* bot, uint8& outHouseId, AuctionHouseObject*& outAh)
    {
        outAh = nullptr;
        if (!bot)
            return false;

        // Mirror the earlier behavior: neutral if two-side interaction enabled.
        outHouseId = ChooseHouseId(bot);
        outAh = sAuctionMgr->GetAuctionsMapByHouseId(outHouseId);
        return outAh != nullptr;
    }

    bool FindAuctionById(uint32 auctionId, AuctionEntry*& outAuction, AuctionHouseObject*& outHouse, uint8& outHouseId)
    {
        outAuction = nullptr;
        outHouse = nullptr;
        outHouseId = 0;

        uint8 const houses[] = { AUCTIONHOUSE_ALLIANCE, AUCTIONHOUSE_HORDE, AUCTIONHOUSE_NEUTRAL };
        for (uint8 houseId : houses)
        {
            AuctionHouseObject* ah = sAuctionMgr->GetAuctionsMapByHouseId(houseId);
            if (!ah)
                continue;
            if (AuctionEntry* a = ah->GetAuction(auctionId))
            {
                outAuction = a;
                outHouse = ah;
                outHouseId = houseId;
                return true;
            }
        }

        return false;
    }
}

bool AuctionCommandAction::Execute(Event event)
{
    Player* bot = botAI->GetBot();
    Player* owner = event.getOwner();
    if (!bot || !owner)
        return false;

    if (!HasNearbyAuctioneer(bot))
    {
        WhisperTo(bot, owner, "ah: cannot find auctioneer nearby");
        return true;
    }

    std::istringstream in(event.getParam());
    std::string sub;
    in >> sub;
    if (sub.empty())
    {
        WhisperTo(bot, owner, "ah: list mine | sell <itemId> <startbid> <buyout> [count] [hours] | cancel <auctionId>");
        return true;
    }

    if (sub == "list")
    {
        std::string which;
        in >> which;
        if (which != "mine")
        {
            WhisperTo(bot, owner, "ah list mine");
            return true;
        }

        uint32 shown = 0;
        uint32 total = 0;
        uint8 const houses[] = { AUCTIONHOUSE_ALLIANCE, AUCTIONHOUSE_HORDE, AUCTIONHOUSE_NEUTRAL };

        for (uint8 houseId : houses)
        {
            AuctionHouseObject* ah = sAuctionMgr->GetAuctionsMapByHouseId(houseId);
            if (!ah)
                continue;

            for (auto itr = ah->GetAuctionsBegin(); itr != ah->GetAuctionsEnd(); ++itr)
            {
                AuctionEntry* a = itr->second;
                if (!a || a->owner != bot->GetGUID())
                    continue;

                ++total;
                if (shown < 10)
                {
                    std::ostringstream line;
                    line << "auction " << a->Id
                         << " item=" << a->itemEntry
                         << " x" << a->itemCount
                         << " bid=" << a->bid
                         << " buyout=" << a->buyout
                         << " house=" << uint32(a->houseId);
                    WhisperTo(bot, owner, line.str());
                    ++shown;
                }
            }
        }

        std::ostringstream out;
        out << "ah mine: " << total << " total (showing " << shown << ")";
        WhisperTo(bot, owner, out.str());
        return true;
    }

    if (sub == "search")
    {
        std::string sItem;
        in >> sItem;
        uint32 itemEntry = ParseU32(sItem);
        if (!itemEntry)
        {
            WhisperTo(bot, owner, "ah search <itemId>");
            return true;
        }

        uint32 shown = 0;
        uint32 total = 0;
        uint8 const houses[] = { AUCTIONHOUSE_ALLIANCE, AUCTIONHOUSE_HORDE, AUCTIONHOUSE_NEUTRAL };

        for (uint8 houseId : houses)
        {
            AuctionHouseObject* ah = sAuctionMgr->GetAuctionsMapByHouseId(houseId);
            if (!ah)
                continue;

            for (auto itr = ah->GetAuctionsBegin(); itr != ah->GetAuctionsEnd(); ++itr)
            {
                AuctionEntry* a = itr->second;
                if (!a || a->itemEntry != itemEntry)
                    continue;

                ++total;
                if (shown < 10)
                {
                    std::ostringstream line;
                    line << "auction " << a->Id
                         << " owner=" << a->owner.GetCounter()
                         << " x" << a->itemCount
                         << " bid=" << a->bid
                         << " buyout=" << a->buyout
                         << " house=" << uint32(a->houseId);
                    WhisperTo(bot, owner, line.str());
                    ++shown;
                }
            }
        }

        std::ostringstream out;
        out << "ah search: " << total << " found (showing " << shown << ")";
        WhisperTo(bot, owner, out.str());
        return true;
    }

    if (sub == "bid" || sub == "buyout")
    {
        std::string sAuc, sPrice;
        in >> sAuc >> sPrice;
        uint32 auctionId = ParseU32(sAuc);
        uint64 price = (sub == "bid") ? ParseU64(sPrice) : 0;

        if (!auctionId || (sub == "bid" && !price))
        {
            WhisperTo(bot, owner, sub == "bid" ? "ah bid <auctionId> <price>" : "ah buyout <auctionId>");
            return true;
        }

        uint8 accessibleHouseId = 0;
        AuctionHouseObject* accessibleAh = nullptr;
        if (!FindAccessibleAuctionHouse(bot, accessibleHouseId, accessibleAh))
        {
            WhisperTo(bot, owner, "ah: auction house not available");
            return true;
        }

        AuctionEntry* auction = nullptr;
        AuctionHouseObject* auctionHouse = nullptr;
        uint8 houseId = 0;
        if (!FindAuctionById(auctionId, auction, auctionHouse, houseId) || !auction || !auctionHouse)
        {
            WhisperTo(bot, owner, "ah: auction not found");
            return true;
        }

        // Must be accessible via current faction rules.
        if (houseId != accessibleHouseId)
        {
            WhisperTo(bot, owner, "ah: auction is in a different auction house (not accessible here)");
            return true;
        }

        if (auction->owner == bot->GetGUID())
        {
            WhisperTo(bot, owner, "ah: cannot bid your own auction");
            return true;
        }

        uint64 finalPrice = price;
        bool buyout = (sub == "buyout") || (auction->buyout != 0 && price >= auction->buyout);
        if (sub == "buyout")
        {
            if (!auction->buyout)
            {
                WhisperTo(bot, owner, "ah buyout: auction has no buyout");
                return true;
            }
            finalPrice = auction->buyout;
            buyout = true;
        }
        else if (buyout)
        {
            // Bid price reaches/exceeds buyout; treat as buyout (like the client).
            finalPrice = auction->buyout;
        }

        if (!buyout)
        {
            if (finalPrice <= auction->bid || finalPrice < auction->startbid)
            {
                WhisperTo(bot, owner, "ah bid: price too low");
                return true;
            }
            if ((finalPrice < auction->buyout || auction->buyout == 0) && finalPrice < auction->bid + auction->GetAuctionOutBid())
            {
                WhisperTo(bot, owner, "ah bid: must outbid by more");
                return true;
            }
        }

        if (!bot->HasEnoughMoney(finalPrice))
        {
            WhisperTo(bot, owner, "ah: not enough money");
            return true;
        }

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

        if (!buyout)
        {
            if (!auction->bidder.IsEmpty())
            {
                if (auction->bidder == bot->GetGUID())
                    bot->ModifyMoney(-int64(finalPrice - auction->bid));
                else
                {
                    sAuctionMgr->SendAuctionOutbiddedMail(auction, finalPrice, bot, trans);
                    bot->ModifyMoney(-int64(finalPrice));
                }
            }
            else
            {
                bot->ModifyMoney(-int64(finalPrice));
            }

            auction->bidder = bot->GetGUID();
            auction->bid = finalPrice;
            auction->SaveToDB(trans);
            bot->SaveInventoryAndGoldToDB(trans);
            CharacterDatabase.CommitTransaction(trans);

            WhisperTo(bot, owner, "ah bid: ok");
            return true;
        }

        // Buyout.
        if (bot->GetGUID().GetCounter() == auction->bidder)
            bot->ModifyMoney(-int64(auction->buyout - auction->bid));
        else
        {
            bot->ModifyMoney(-int64(auction->buyout));
            if (!auction->bidder.IsEmpty())
                sAuctionMgr->SendAuctionOutbiddedMail(auction, auction->buyout, bot, trans);
        }

        auction->bidder = bot->GetGUID();
        auction->bid = auction->buyout;

        sAuctionMgr->SendAuctionSalePendingMail(auction, trans);
        sAuctionMgr->SendAuctionSuccessfulMail(auction, trans);
        sAuctionMgr->SendAuctionWonMail(auction, trans);

        auction->DeleteFromDB(trans);
        sAuctionMgr->RemoveAItem(auction->itemGUIDLow);
        auctionHouse->RemoveAuction(auction);

        bot->SaveInventoryAndGoldToDB(trans);
        CharacterDatabase.CommitTransaction(trans);

        WhisperTo(bot, owner, "ah buyout: ok");
        return true;
    }

    if (sub == "sell")
    {
        std::string sItem, sBid, sBuyout, sCount, sHours;
        in >> sItem >> sBid >> sBuyout >> sCount >> sHours;

        uint32 itemEntry = ParseU32(sItem);
        uint64 startbid = ParseU64(sBid);
        uint64 buyout = ParseU64(sBuyout);
        uint32 count = sCount.empty() ? 1u : ParseU32(sCount);
        uint32 hours = sHours.empty() ? 24u : ParseU32(sHours);

        if (!itemEntry || !startbid || !buyout || !count)
        {
            WhisperTo(bot, owner, "ah sell <itemId> <startbid> <buyout> [count] [hours]");
            return true;
        }

        if (buyout > MAX_MONEY_AMOUNT || startbid > MAX_MONEY_AMOUNT)
        {
            WhisperTo(bot, owner, "ah sell: invalid gold amount");
            return true;
        }

        if (hours != 12 && hours != 24 && hours != 48)
        {
            WhisperTo(bot, owner, "ah sell: hours must be 12/24/48");
            return true;
        }

        uint32 etime = hours * HOUR;

        Item* item = FindSellItem(bot, itemEntry, count);
        if (!item)
        {
            WhisperTo(bot, owner, "ah sell: item not found in bags");
            return true;
        }

        if (sAuctionMgr->GetAItem(item->GetGUID().GetCounter()) || !item->CanBeTraded() || item->IsNotEmptyBag() ||
            item->GetTemplate()->Flags & ITEM_PROTO_FLAG_CONJURED || item->GetUInt32Value(ITEM_FIELD_EXPIRATION))
        {
            WhisperTo(bot, owner, "ah sell: item cannot be auctioned");
            return true;
        }

        uint8 houseId = ChooseHouseId(bot);
        AuctionHouseEntry const* auctionHouseEntry = AuctionHouseMgr::GetAuctionHouseEntryFromHouse(houseId);
        if (!auctionHouseEntry)
        {
            WhisperTo(bot, owner, "ah sell: auction house entry not found");
            return true;
        }

        uint64 deposit = sAuctionMgr->GetAuctionDeposit(auctionHouseEntry, etime, item, count);
        if (!bot->HasEnoughMoney(deposit))
        {
            WhisperTo(bot, owner, "ah sell: not enough money for deposit");
            return true;
        }

        uint32 auctionTime = uint32(etime * sWorld->getRate(RATE_AUCTION_TIME));
        AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMapByHouseId(houseId);

        AuctionEntry* AH = new AuctionEntry();
        AH->Id = sObjectMgr->GenerateAuctionID();
        AH->houseId = houseId;
        AH->itemEntry = item->GetEntry();
        AH->itemCount = count;
        AH->owner = bot->GetGUID();
        AH->startbid = startbid;
        AH->bidder = ObjectGuid::Empty;
        AH->bid = 0;
        AH->buyout = buyout;
        AH->expire_time = GameTime::GetGameTime() + auctionTime;
        AH->deposit = deposit;
        AH->etime = etime;
        AH->auctionHouseEntry = auctionHouseEntry;

        // Single-stack only (no multi-stack combine like the client allows).
        if (item->GetCount() == count)
        {
            AH->itemGUIDLow = item->GetGUID().GetCounter();

            if (!sAuctionMgr->PendingAuctionAdd(bot, AH, item))
            {
                delete AH;
                WhisperTo(bot, owner, "ah sell: pending add failed (deposit?)");
                return true;
            }

            sAuctionMgr->AddAItem(item);
            auctionHouse->AddAuction(AH);
            bot->MoveItemFromInventory(item->GetBagSlot(), item->GetSlot(), true);

            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            item->DeleteFromInventoryDB(trans);
            item->SaveToDB(trans);
            AH->SaveToDB(trans);
            bot->SaveInventoryAndGoldToDB(trans);
            CharacterDatabase.CommitTransaction(trans);
        }
        else
        {
            Item* newItem = item->CloneItem(count, bot);
            if (!newItem)
            {
                delete AH;
                WhisperTo(bot, owner, "ah sell: clone failed");
                return true;
            }

            AH->itemGUIDLow = newItem->GetGUID().GetCounter();

            if (!sAuctionMgr->PendingAuctionAdd(bot, AH, newItem))
            {
                delete AH;
                delete newItem;
                WhisperTo(bot, owner, "ah sell: pending add failed (deposit?)");
                return true;
            }

            sAuctionMgr->AddAItem(newItem);
            auctionHouse->AddAuction(AH);

            item->SetCount(item->GetCount() - count);
            item->SetState(ITEM_CHANGED, bot);
            item->SendUpdateToPlayer(bot);

            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            item->SaveToDB(trans);
            newItem->SaveToDB(trans);
            AH->SaveToDB(trans);
            bot->SaveInventoryAndGoldToDB(trans);
            CharacterDatabase.CommitTransaction(trans);
        }

        std::ostringstream out;
        out << "ah sell: auction " << AH->Id << " created (deposit " << deposit << ")";
        WhisperTo(bot, owner, out.str());
        return true;
    }

    if (sub == "cancel")
    {
        std::string arg;
        in >> arg;
        uint32 auctionId = ParseU32(arg);
        if (!auctionId)
        {
            WhisperTo(bot, owner, "ah cancel <auctionId>");
            return true;
        }

        uint8 const houses[] = { AUCTIONHOUSE_ALLIANCE, AUCTIONHOUSE_HORDE, AUCTIONHOUSE_NEUTRAL };
        AuctionHouseObject* auctionHouse = nullptr;
        AuctionEntry* auction = nullptr;
        for (uint8 houseId : houses)
        {
            AuctionHouseObject* ah = sAuctionMgr->GetAuctionsMapByHouseId(houseId);
            if (!ah)
                continue;
            if (AuctionEntry* a = ah->GetAuction(auctionId))
            {
                auctionHouse = ah;
                auction = a;
                break;
            }
        }

        if (!auction || !auctionHouse || auction->owner != bot->GetGUID())
        {
            WhisperTo(bot, owner, "ah cancel: not found / not owner");
            return true;
        }

        Item* pItem = sAuctionMgr->GetAItem(auction->itemGUIDLow);
        if (!pItem)
        {
            WhisperTo(bot, owner, "ah cancel: auction item missing");
            return true;
        }

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

        if (!auction->bidder.IsEmpty())
        {
            uint64 auctionCut = auction->GetAuctionCut();
            if (!bot->HasEnoughMoney(auctionCut))
            {
                WhisperTo(bot, owner, "ah cancel: not enough money for cut");
                return true;
            }
            sAuctionMgr->SendAuctionCancelledToBidderMail(auction, trans, pItem);
            bot->ModifyMoney(-int64(auctionCut));
        }

        MailDraft(auction->BuildAuctionMailSubject(AUCTION_CANCELED),
                  AuctionEntry::BuildAuctionMailBody(ObjectGuid::Empty, 0, auction->buyout, auction->deposit, 0))
            .AddItem(pItem)
            .SendMailTo(trans, bot, auction, MAIL_CHECK_MASK_COPIED);

        bot->SaveInventoryAndGoldToDB(trans);
        auction->DeleteFromDB(trans);
        CharacterDatabase.CommitTransaction(trans);

        sAuctionMgr->RemoveAItem(auction->itemGUIDLow);
        auctionHouse->RemoveAuction(auction);

        WhisperTo(bot, owner, "ah cancel: ok");
        return true;
    }

    WhisperTo(bot, owner, "ah: unknown subcommand");
    return true;
}
