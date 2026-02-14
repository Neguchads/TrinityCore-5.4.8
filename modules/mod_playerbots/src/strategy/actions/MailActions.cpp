#include "MailActions.h"

#include <cstdlib>
#include <sstream>

#include "CellImpl.h"
#include "Chat.h"
#include "CharacterCache.h"
#include "GameObject.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Bag.h"
#include "Item.h"
#include "Mail.h"
#include "ObjectAccessor.h"
#include "ObjectDefines.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "World.h"

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

    bool IsDeliverable(Mail const* m)
    {
        if (!m)
            return false;
        if (m->state == MAIL_STATE_DELETED)
            return false;
        if (m->deliver_time > time(nullptr))
            return false;
        return true;
    }

    bool HasNearbyMailbox(Player* bot)
    {
        if (!bot)
            return false;

        float const range = INTERACTION_DISTANCE * 2.0f;

        // NPC mailbox.
        {
            std::list<Unit*> units;
            Trinity::AnyUnitInObjectRangeCheck u_check(bot, range);
            Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher(bot, units, u_check);
            Cell::VisitAllObjects(bot, searcher, range);

            for (Unit* u : units)
            {
                Creature* c = u ? u->ToCreature() : nullptr;
                if (!c)
                    continue;
                if (bot->GetNPCIfCanInteractWith(c->GetGUID(), UNIT_NPC_FLAG_MAILBOX))
                    return true;
            }
        }

        // GO mailbox.
        {
            struct AnyGameObjectInObjectRangeCheck
            {
                WorldObject const* obj;
                float range;
                AnyGameObjectInObjectRangeCheck(WorldObject const* obj, float range) : obj(obj), range(range) {}
                bool operator()(GameObject* go) const { return go && go->IsWithinDistInMap(obj, range); }
            };

            std::list<GameObject*> gos;
            AnyGameObjectInObjectRangeCheck go_check(bot, range);
            Trinity::GameObjectListSearcher<AnyGameObjectInObjectRangeCheck> searcher(bot, gos, go_check);
            Cell::VisitAllObjects(bot, searcher, range);

            for (GameObject* go : gos)
            {
                if (!go)
                    continue;
                if (bot->GetGameObjectIfCanInteractWith(go->GetGUID(), GAMEOBJECT_TYPE_MAILBOX))
                    return true;
            }
        }

        return false;
    }

    Item* FindSendItem(Player* bot, uint32 itemEntry, uint32 count)
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
}

bool MailCommandAction::Execute(Event event)
{
    Player* bot = botAI->GetBot();
    Player* owner = event.getOwner();
    if (!bot || !owner)
        return false;

    if (!HasNearbyMailbox(bot))
    {
        WhisperTo(bot, owner, "mail: cannot find mailbox nearby");
        return true;
    }

    if (!bot->m_mailsLoaded)
    {
        WhisperTo(bot, owner, "mail: mails not loaded yet");
        return true;
    }

    std::istringstream in(event.getParam());
    std::string sub;
    in >> sub;

    if (sub.empty())
    {
        WhisperTo(bot, owner, "mail: list | take money <all|mailId> | take items <all|mailId> | delete <mailId>");
        return true;
    }

    if (sub == "list")
    {
        uint32 shown = 0;
        uint32 total = 0;

        for (auto it = bot->GetMailBegin(); it != bot->GetMailEnd(); ++it)
        {
            Mail* m = *it;
            if (!m)
                continue;

            ++total;
            if (shown >= 10)
                continue;

            std::ostringstream line;
            line << "mail " << m->messageID
                 << " items=" << m->items.size()
                 << " money=" << m->money
                 << " cod=" << m->COD;

            if (!m->subject.empty())
                line << " subj=\"" << m->subject << "\"";

            WhisperTo(bot, owner, line.str());
            ++shown;
        }

        std::ostringstream out;
        out << "mail: " << total << " total (showing " << shown << ")";
        WhisperTo(bot, owner, out.str());
        return true;
    }

    if (sub == "send")
    {
        std::string what;
        in >> what;
        if (what.empty())
        {
            WhisperTo(bot, owner, "mail send: money <player> <amount> [subject] | item <player> <itemId> [count] [subject]");
            return true;
        }

        std::string receiverName;
        in >> receiverName;
        if (receiverName.empty())
        {
            WhisperTo(bot, owner, "mail send: missing receiver");
            return true;
        }

        ObjectGuid receiverGuid = sCharacterCache->GetCharacterGuidByName(receiverName);
        if (receiverGuid.IsEmpty())
        {
            WhisperTo(bot, owner, "mail send: receiver not found");
            return true;
        }

        if (receiverGuid == bot->GetGUID())
        {
            WhisperTo(bot, owner, "mail send: cannot send to self");
            return true;
        }

        CharacterCacheEntry const* cache = sCharacterCache->GetCharacterCacheByGuid(receiverGuid);
        if (!cache)
        {
            sCharacterCache->RefreshCacheEntry(receiverGuid.GetCounter());
            cache = sCharacterCache->GetCharacterCacheByGuid(receiverGuid);
        }
        uint32 receiverAccountId = cache ? cache->AccountId : 0;

        std::string rest;
        std::getline(in, rest);
        while (!rest.empty() && std::isspace(static_cast<unsigned char>(rest[0])))
            rest.erase(rest.begin());

        if (what == "money")
        {
            std::istringstream args(rest);
            std::string sAmount;
            args >> sAmount;
            uint64 amount = ParseU64(sAmount);
            if (!amount)
            {
                WhisperTo(bot, owner, "mail send money <player> <amount> [subject]");
                return true;
            }

            if (amount > bot->GetMoney())
            {
                WhisperTo(bot, owner, "mail send money: not enough gold");
                return true;
            }

            std::string subject;
            std::getline(args, subject);
            while (!subject.empty() && std::isspace(static_cast<unsigned char>(subject[0])))
                subject.erase(subject.begin());
            if (subject.empty())
                subject = "Playerbots";

            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            MailDraft(subject, "")
                .AddMoney(amount)
                .SendMailTo(trans,
                    MailReceiver(ObjectAccessor::FindConnectedPlayer(receiverGuid), receiverGuid.GetCounter()),
                    MailSender(bot));

            bot->ModifyMoney(-(int64)amount);
            bot->SaveInventoryAndGoldToDB(trans);
            CharacterDatabase.CommitTransaction(trans);

            WhisperTo(bot, owner, "mail send money: ok");
            return true;
        }

        if (what == "item")
        {
            std::istringstream args(rest);
            std::string sItem, sCount;
            args >> sItem >> sCount;

            uint32 itemEntry = ParseU32(sItem);
            uint32 count = sCount.empty() ? 1u : ParseU32(sCount);
            if (!itemEntry || !count)
            {
                WhisperTo(bot, owner, "mail send item <player> <itemId> [count] [subject]");
                return true;
            }

            Item* item = FindSendItem(bot, itemEntry, count);
            if (!item)
            {
                WhisperTo(bot, owner, "mail send item: item not found in bags");
                return true;
            }

            if (!item->CanBeTraded() || item->IsNotEmptyBag() || (item->GetTemplate()->Flags & ITEM_PROTO_FLAG_CONJURED))
            {
                WhisperTo(bot, owner, "mail send item: item cannot be mailed");
                return true;
            }

            std::string subject;
            std::getline(args, subject);
            while (!subject.empty() && std::isspace(static_cast<unsigned char>(subject[0])))
                subject.erase(subject.begin());
            if (subject.empty())
                subject = "Playerbots";

            bool needItemDelay = receiverAccountId && (receiverAccountId != bot->GetSession()->GetAccountId());
            uint32 deliverDelay = needItemDelay ? sWorld->getIntConfig(CONFIG_MAIL_DELIVERY_DELAY) : 0;
            if (Guild* guild = sGuildMgr->GetGuildById(bot->GetGuildId()))
                if (guild->GetLevel() >= 17 && guild->IsMember(receiverGuid))
                    deliverDelay = 0;

            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            MailDraft draft(subject, "");

            if (item->GetCount() == count)
            {
                bot->MoveItemFromInventory(item->GetBagSlot(), item->GetSlot(), true);
                item->DeleteFromInventoryDB(trans);
                item->SetOwnerGUID(receiverGuid);
                item->SaveToDB(trans);
                draft.AddItem(item);
            }
            else
            {
                Item* newItem = item->CloneItem(count, bot);
                if (!newItem)
                {
                    CharacterDatabase.CommitTransaction(trans);
                    WhisperTo(bot, owner, "mail send item: clone failed");
                    return true;
                }

                item->SetCount(item->GetCount() - count);
                item->SetState(ITEM_CHANGED, bot);
                item->SendUpdateToPlayer(bot);

                newItem->SetOwnerGUID(receiverGuid);
                newItem->SaveToDB(trans);
                item->SaveToDB(trans);
                draft.AddItem(newItem);
            }

            draft.SendMailTo(trans,
                MailReceiver(ObjectAccessor::FindConnectedPlayer(receiverGuid), receiverGuid.GetCounter()),
                MailSender(bot),
                MAIL_CHECK_MASK_COPIED,
                deliverDelay);

            bot->SaveInventoryAndGoldToDB(trans);
            CharacterDatabase.CommitTransaction(trans);

            WhisperTo(bot, owner, "mail send item: ok");
            return true;
        }

        WhisperTo(bot, owner, "mail send: unknown target (money/item)");
        return true;
    }

    if (sub == "take")
    {
        std::string what;
        in >> what;
        std::string arg;
        in >> arg;

        if (what.empty() || arg.empty())
        {
            WhisperTo(bot, owner, "mail take money <all|mailId> | mail take items <all|mailId>");
            return true;
        }

        uint32 targetMailId = 0;
        bool all = (arg == "all");
        if (!all)
        {
            targetMailId = ParseU32(arg);
            if (!targetMailId)
            {
                WhisperTo(bot, owner, "mail take: invalid mailId");
                return true;
            }
        }

        if (what == "money")
        {
            uint32 taken = 0;
            for (auto it = bot->GetMailBegin(); it != bot->GetMailEnd(); ++it)
            {
                Mail* m = *it;
                if (!IsDeliverable(m))
                    continue;
                if (!all && m->messageID != targetMailId)
                    continue;
                if (!m->money)
                    continue;

                if (!bot->ModifyMoney(m->money, false))
                {
                    WhisperTo(bot, owner, "mail take money: too much gold");
                    break;
                }

                m->money = 0;
                m->state = MAIL_STATE_CHANGED;
                bot->m_mailsUpdated = true;
                ++taken;
            }

            if (bot->m_mailsUpdated)
                bot->SaveToDB(false);

            std::ostringstream out;
            out << "mail take money: " << taken;
            WhisperTo(bot, owner, out.str());
            return true;
        }

        if (what == "items")
        {
            uint32 moved = 0;
            for (auto it = bot->GetMailBegin(); it != bot->GetMailEnd(); ++it)
            {
                Mail* m = *it;
                if (!IsDeliverable(m))
                    continue;
                if (!all && m->messageID != targetMailId)
                    continue;
                if (m->COD)
                    continue; // keep it simple (no COD payments) for now

                // Iterate a copy because RemoveItem mutates the vector.
                std::vector<MailItemInfo> items = m->items;
                for (MailItemInfo const& mi : items)
                {
                    Item* itItem = bot->GetMItem(mi.item_guid);
                    if (!itItem)
                        continue;

                    ItemPosCountVec dest;
                    uint8 msg = bot->CanStoreItem(NULL_BAG, NULL_SLOT, dest, itItem, false);
                    if (msg != EQUIP_ERR_OK)
                        continue;

                    m->RemoveItem(mi.item_guid);
                    m->removedItems.push_back(mi.item_guid);
                    m->state = MAIL_STATE_CHANGED;
                    bot->m_mailsUpdated = true;

                    bot->RemoveMItem(mi.item_guid);

                    itItem->SetState(ITEM_UNCHANGED);
                    bot->MoveItemToInventory(dest, itItem, true);

                    ++moved;
                }
            }

            if (bot->m_mailsUpdated)
                bot->SaveToDB(false);

            std::ostringstream out;
            out << "mail take items: " << moved;
            WhisperTo(bot, owner, out.str());
            return true;
        }

        WhisperTo(bot, owner, "mail take: unknown target (money/items)");
        return true;
    }

    if (sub == "delete")
    {
        std::string arg;
        in >> arg;
        uint32 mailId = ParseU32(arg);
        if (!mailId)
        {
            WhisperTo(bot, owner, "mail delete <mailId>");
            return true;
        }

        Mail* m = bot->GetMail(mailId);
        if (!IsDeliverable(m))
        {
            WhisperTo(bot, owner, "mail delete: mail not found / not delivered");
            return true;
        }

        if (m->COD || m->money || m->HasItems())
        {
            WhisperTo(bot, owner, "mail delete: mail has COD/money/items");
            return true;
        }

        m->state = MAIL_STATE_DELETED;
        bot->m_mailsUpdated = true;
        bot->SaveToDB(false);
        WhisperTo(bot, owner, "mail delete: ok");
        return true;
    }

    WhisperTo(bot, owner, "mail: unknown subcommand");
    return true;
}
