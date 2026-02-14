#include "GuildCreateActions.h"

#include <algorithm>
#include <sstream>

#include "Bag.h"
#include "ChatHelper.h"
#include "DatabaseEnv.h"
#include "DBCEnums.h"
#include "DBCStores.h"
#include "Creature.h"
#include "Event.h"
#include "Item.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "World.h"
#include "WorldSession.h"

#include "Factory/RandomPlayerbotFactory.h"

namespace
{
uint32 constexpr GUILD_CHARTER_ITEM = 5863;
uint32 constexpr GUILD_TABARD_ITEM = 5976;
uint32 constexpr GUILD_CHARTER_COST = 1000;

Item* FindFirstItemByEntry(Player* bot, uint32 entry)
{
    if (!bot || !entry)
        return nullptr;

    // Backpack.
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
        if (Item* it = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
            if (it->GetEntry() == entry)
                return it;

    // Bags.
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

bool BuildPetitionBuyPacket(WorldPacket& out, ObjectGuid npcGuid, std::string const& name)
{
    if (!npcGuid || name.empty() || name.size() > 127)
        return false;

    out.Initialize(CMSG_PETITION_BUY);

    out.WriteBit(npcGuid[5]);
    out.WriteBit(npcGuid[2]);
    out.WriteBit(npcGuid[3]);
    out.WriteBits(uint32(name.size()), 7);
    out.WriteBit(npcGuid[4]);
    out.WriteBit(npcGuid[1]);
    out.WriteBit(npcGuid[7]);
    out.WriteBit(npcGuid[0]);
    out.WriteBit(npcGuid[6]);
    out.FlushBits();

    out.WriteString(name);

    out.WriteByteSeq(npcGuid[1]);
    out.WriteByteSeq(npcGuid[7]);
    out.WriteByteSeq(npcGuid[4]);
    out.WriteByteSeq(npcGuid[6]);
    out.WriteByteSeq(npcGuid[0]);
    out.WriteByteSeq(npcGuid[5]);
    out.WriteByteSeq(npcGuid[2]);
    out.WriteByteSeq(npcGuid[3]);

    return true;
}

bool BuildOfferPetitionPacket(WorldPacket& out, ObjectGuid petitionGuid, ObjectGuid playerGuid)
{
    if (!petitionGuid || !playerGuid)
        return false;

    out.Initialize(CMSG_OFFER_PETITION);
    out << uint32(0); // unused (handler read_skip<uint32>())

    out.WriteBit(playerGuid[4]);
    out.WriteBit(playerGuid[1]);
    out.WriteBit(petitionGuid[2]);
    out.WriteBit(playerGuid[6]);
    out.WriteBit(petitionGuid[1]);
    out.WriteBit(playerGuid[2]);
    out.WriteBit(petitionGuid[4]);
    out.WriteBit(playerGuid[3]);
    out.WriteBit(playerGuid[7]);
    out.WriteBit(petitionGuid[0]);
    out.WriteBit(petitionGuid[6]);
    out.WriteBit(playerGuid[5]);
    out.WriteBit(playerGuid[0]);
    out.WriteBit(petitionGuid[3]);
    out.WriteBit(petitionGuid[5]);
    out.WriteBit(petitionGuid[7]);
    out.FlushBits();

    out.WriteByteSeq(playerGuid[7]);
    out.WriteByteSeq(petitionGuid[1]);
    out.WriteByteSeq(petitionGuid[4]);
    out.WriteByteSeq(petitionGuid[2]);
    out.WriteByteSeq(playerGuid[6]);
    out.WriteByteSeq(petitionGuid[3]);
    out.WriteByteSeq(petitionGuid[0]);
    out.WriteByteSeq(petitionGuid[5]);
    out.WriteByteSeq(playerGuid[0]);
    out.WriteByteSeq(playerGuid[2]);
    out.WriteByteSeq(playerGuid[5]);
    out.WriteByteSeq(playerGuid[3]);
    out.WriteByteSeq(playerGuid[4]);
    out.WriteByteSeq(petitionGuid[7]);
    out.WriteByteSeq(playerGuid[1]);
    out.WriteByteSeq(petitionGuid[6]);

    return true;
}

bool BuildTurnInPetitionPacket(WorldPacket& out, ObjectGuid petitionGuid)
{
    if (!petitionGuid)
        return false;

    out.Initialize(CMSG_TURN_IN_PETITION);

    out.WriteBit(petitionGuid[1]);
    out.WriteBit(petitionGuid[2]);
    out.WriteBit(petitionGuid[3]);
    out.WriteBit(petitionGuid[0]);
    out.WriteBit(petitionGuid[5]);
    out.WriteBit(petitionGuid[7]);
    out.WriteBit(petitionGuid[4]);
    out.WriteBit(petitionGuid[6]);
    out.FlushBits();

    out.WriteByteSeq(petitionGuid[2]);
    out.WriteByteSeq(petitionGuid[1]);
    out.WriteByteSeq(petitionGuid[4]);
    out.WriteByteSeq(petitionGuid[6]);
    out.WriteByteSeq(petitionGuid[0]);
    out.WriteByteSeq(petitionGuid[7]);
    out.WriteByteSeq(petitionGuid[5]);
    out.WriteByteSeq(petitionGuid[3]);

    return true;
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

uint32 FindVendorSlotForItem(Creature* vendor, uint32 itemEntry)
{
    if (!vendor || !itemEntry)
        return uint32(-1);

    VendorItemData const* items = vendor->GetVendorItems();
    if (!items || items->Empty())
        return uint32(-1);

    for (uint32 i = 0; i < items->GetItemCount(); ++i)
    {
        VendorItem const* v = items->GetItem(i);
        if (v && v->Type == ITEM_VENDOR_TYPE_ITEM && v->item == itemEntry)
            return i;
    }

    return uint32(-1);
}
}

bool BuyPetitionAction::CanBuyPetition(Player* bot)
{
    if (!bot)
        return false;

    if (!sPlayerbotAIConfig->randomBotFormGuild)
        return false;

    if (bot->GetGuildId() || bot->GetGuildIdInvited())
        return false;

    if (FindFirstItemByEntry(bot, GUILD_CHARTER_ITEM))
        return false;

    if (bot->GetMoney() < GUILD_CHARTER_COST)
        return false;

    return true;
}

bool BuyPetitionAction::isUseful()
{
    if (!bot || !sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    if (botAI->HasActivePlayerMaster())
        return false;

    return CanBuyPetition(bot);
}

bool BuyPetitionAction::Execute(Event /*event*/)
{
    if (!CanBuyPetition(bot))
        return false;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest npcs");
    for (ObjectGuid const& guid : npcs)
    {
        Creature* creature = bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_PETITIONER);
        if (!creature)
            continue;

        std::string guildName = RandomPlayerbotFactory::CreateRandomGuildName();
        if (guildName.empty())
            continue;

        WorldPacket data;
        if (!BuildPetitionBuyPacket(data, creature->GetGUID(), guildName))
            return false;

        bot->GetSession()->HandlePetitionBuyOpcode(data);
        return true;
    }

    return false;
}

bool PetitionOfferAction::isUseful()
{
    if (!bot || !sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    if (!sPlayerbotAIConfig->randomBotFormGuild)
        return false;

    if (botAI->HasActivePlayerMaster() || botAI->IsAlt())
        return false;

    if (bot->GetGuildId())
        return false;

    return FindFirstItemByEntry(bot, GUILD_CHARTER_ITEM) != nullptr;
}

bool PetitionOfferAction::Execute(Event event)
{
    Item* petition = FindFirstItemByEntry(bot, GUILD_CHARTER_ITEM);
    if (!petition)
        return false;

    ObjectGuid targetGuid = event.getObject();
    if (!targetGuid || !targetGuid.IsPlayer())
        if (Player* selected = bot->GetSelectedPlayer())
            targetGuid = selected->GetGUID();

    if (!targetGuid || !targetGuid.IsPlayer())
        return false;

    Player* player = ObjectAccessor::FindPlayer(targetGuid);
    if (!player || player == bot)
        return false;

    // Avoid offering to players that already signed this petition (same account).
    if (player->GetSession())
    {
        QueryResult r = CharacterDatabase.PQuery(
            "SELECT playerguid FROM petition_sign WHERE player_account = '%u' AND petitionguid = '%u' LIMIT 1",
            player->GetSession()->GetAccountId(), petition->GetGUID().GetCounter());
        if (r)
            return false;
    }

    WorldPacket data;
    if (!BuildOfferPetitionPacket(data, petition->GetGUID(), player->GetGUID()))
        return false;

    bot->GetSession()->HandleOfferPetitionOpcode(data);
    return true;
}

bool PetitionOfferNearbyAction::isUseful()
{
    if (!PetitionOfferAction::isUseful())
        return false;

    // Only offer if not enough signatures yet.
    uint8 signs = AI_VALUE(uint8, "petition signs");
    uint32 required = sWorld->getIntConfig(CONFIG_MIN_PETITION_SIGNS);
    return signs < required;
}

bool PetitionOfferNearbyAction::Execute(Event /*event*/)
{
    Item* petition = FindFirstItemByEntry(bot, GUILD_CHARTER_ITEM);
    if (!petition)
        return false;

    uint32 offered = 0;
    GuidVector nearGuids = AI_VALUE(GuidVector, "nearest friendly players");
    for (ObjectGuid const& guid : nearGuids)
    {
        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player || player == bot)
            continue;

        if (player->GetGuildId() || player->GetGuildIdInvited())
            continue;

        if (!sPlayerbotAIConfig->randomBotInvitePlayer && player->IsPlayer() && !GET_PLAYERBOT_AI(player))
            continue;

        if (PlayerbotAI* otherAI = GET_PLAYERBOT_AI(player))
            if (otherAI->HasActivePlayerMaster())
                continue;

        if (bot->GetDistance2d(player) > sPlayerbotAIConfig->sightDistance)
            continue;

        if (sPlayerbotAIConfig->inviteChat &&
            bot->GetDistance2d(player) < sPlayerbotAIConfig->spellDistance &&
            (sRandomPlayerbotMgr->IsRandomBot(bot) || !botAI->HasActivePlayerMaster()))
        {
            bot->Say("Want to form a guild together?", LANG_UNIVERSAL);
        }

        Event petitionOfferEvent("petition offer nearby", guid, player);
        if (PetitionOfferAction::Execute(petitionOfferEvent))
        {
            ++offered;
            if (offered >= 1)
                break;
        }
    }

    return offered > 0;
}

bool PetitionTurnInAction::isUseful()
{
    if (!bot || !sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    if (!sPlayerbotAIConfig->randomBotFormGuild)
        return false;

    if (botAI->HasActivePlayerMaster() || botAI->IsAlt())
        return false;

    return AI_VALUE(bool, "can hand in petition");
}

bool PetitionTurnInAction::Execute(Event /*event*/)
{
    Item* petition = FindFirstItemByEntry(bot, GUILD_CHARTER_ITEM);
    if (!petition)
        return false;

    WorldPacket data;
    if (!BuildTurnInPetitionPacket(data, petition->GetGUID()))
        return false;

    bot->GetSession()->HandleTurnInPetitionOpcode(data);
    return true;
}

bool BuyTabardAction::isUseful()
{
    if (!bot || !sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    if (botAI->HasActivePlayerMaster() || botAI->IsAlt())
        return false;

    return AI_VALUE(bool, "can buy tabard");
}

bool BuyTabardAction::Execute(Event /*event*/)
{
    if (!bot || bot->GetGuildId() == 0)
        return false;

    if (FindFirstItemByEntry(bot, GUILD_TABARD_ITEM))
        return true;

    if (!IsInCapitalCity(bot))
        return false;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest npcs");
    for (ObjectGuid const& guid : npcs)
    {
        Creature* vendor = bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_VENDOR);
        if (!vendor)
            continue;

        uint32 slot = FindVendorSlotForItem(vendor, GUILD_TABARD_ITEM);
        if (slot == uint32(-1))
            continue;

        return bot->BuyItemFromVendorSlot(vendor->GetGUID(), slot, GUILD_TABARD_ITEM, 1, INVENTORY_SLOT_BAG_0, NULL_SLOT);
    }

    return false;
}
