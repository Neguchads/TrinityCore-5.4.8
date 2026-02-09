/*
* This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "LootRollAction.h"

#include "Event.h"
#include "Group.h"
#include "ItemPrototype.h"
#include "LootAction.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "SharedDefines.h"

namespace
{
    struct StartRollData
    {
        ObjectGuid lootGuid = ObjectGuid::Empty;
        uint8 itemSlot = 0;
        uint32 itemId = 0;
    };

    StartRollData ParseStartRollPacket(WorldPacket& p)
    {
        StartRollData data;

        p.rpos(0);

        p.ReadBits(3);
        bool noSlot = p.ReadBit();
        p.ReadBit();
        data.lootGuid[3] = p.ReadBit();
        data.lootGuid[1] = p.ReadBit();
        data.lootGuid[7] = p.ReadBit();
        data.lootGuid[6] = p.ReadBit();
        data.lootGuid[2] = p.ReadBit();
        data.lootGuid[4] = p.ReadBit();
        data.lootGuid[5] = p.ReadBit();
        data.lootGuid[0] = p.ReadBit();
        bool noExtra = p.ReadBit();
        p.ReadBits(2);

        p.ReadByteSeq(data.lootGuid[7]);
        p.read_skip<uint32>();
        p.ReadByteSeq(data.lootGuid[5]);
        p.read_skip<uint32>();
        p.read_skip<uint32>();
        if (!noSlot)
            p >> data.itemSlot;
        p.ReadByteSeq(data.lootGuid[4]);
        p.ReadByteSeq(data.lootGuid[0]);
        p.ReadByteSeq(data.lootGuid[3]);
        p.ReadByteSeq(data.lootGuid[2]);
        p.read_skip<uint32>();
        p >> data.itemId;
        p.read_skip<uint8>();
        p.read_skip<uint32>();
        p.read_skip<uint8>();
        p.read_skip<uint32>();
        if (!noExtra)
            p.read_skip<uint8>();
        p.ReadByteSeq(data.lootGuid[6]);
        p.read_skip<uint32>();
        p.ReadByteSeq(data.lootGuid[1]);

        return data;
    }
}

bool LootRollAction::Execute(Event event)
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    WorldPacket p(event.getPacket());
    StartRollData rollData = ParseStartRollPacket(p);
    if (!rollData.lootGuid || !rollData.itemId)
        return false;

    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(rollData.itemId);
    if (!proto)
        return false;

    RollVote vote = PASS;
    if (proto->Class == ITEM_CLASS_WEAPON || proto->Class == ITEM_CLASS_ARMOR)
    {
        vote = (bot->CanUseItem(proto) == EQUIP_ERR_OK) ? NEED : GREED;
    }
    else if (StoreLootAction::IsLootAllowed(rollData.itemId, botAI))
    {
        vote = GREED;
    }

    group->CountRollVote(rollData.lootGuid, bot->GetGUID(), rollData.itemSlot, uint8(vote));
    return true;
}

RollVote LootRollAction::CalculateRollVote(ItemTemplate const* proto)
{
    if (!proto)
        return PASS;

    if (proto->Class == ITEM_CLASS_WEAPON || proto->Class == ITEM_CLASS_ARMOR)
        return (bot->CanUseItem(proto) == EQUIP_ERR_OK) ? NEED : GREED;

    return StoreLootAction::IsLootAllowed(proto->ItemId, GET_PLAYERBOT_AI(bot)) ? GREED : PASS;
}

bool MasterLootRollAction::isUseful() { return !botAI->HasActivePlayerMaster(); }

bool MasterLootRollAction::Execute(Event event)
{
    return LootRollAction::Execute(event);
}

bool RollAction::Execute(Event event)
{
    std::string link = event.getParam();

    if (link.empty())
    {
        bot->DoRandomRoll(0, 100);
        return false;
    }

    bot->DoRandomRoll(0, 100);
    return true;
}
