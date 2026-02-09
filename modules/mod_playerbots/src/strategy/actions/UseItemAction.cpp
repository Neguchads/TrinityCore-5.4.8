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

#include "UseItemAction.h"

#include "Bag.h"
#include "Event.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "SharedDefines.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "WorldPacket.h"

void UseItemAction::IterateItems(Player* bot, IterateItemsVisitor& visitor, uint32 mask)
{
    if (!bot)
        return;

    if (mask & ITERATE_ITEMS_IN_BAGS)
    {
        for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        {
            if (Item* item = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                if (!visitor.Visit(item))
                    return;
            }
        }

        for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
        {
            if (Bag* bag = bot->GetBagByPos(i))
            {
                for (uint32 j = 0; j < bag->GetBagSize(); ++j)
                {
                    if (Item* item = bag->GetItemByPos(j))
                    {
                        if (!visitor.Visit(item))
                            return;
                    }
                }
            }
        }
    }

    if (mask & ITERATE_ITEMS_IN_EQUIP)
    {
        for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
        {
            if (Item* item = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                if (!visitor.Visit(item))
                    return;
            }
        }
    }

    if (mask & ITERATE_ITEMS_IN_BANK)
    {
        for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; ++i)
        {
            if (Item* item = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                if (!visitor.Visit(item))
                    return;
            }
        }

        for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
        {
            if (Bag* bag = bot->GetBagByPos(i))
            {
                for (uint32 j = 0; j < bag->GetBagSize(); ++j)
                {
                    if (Item* item = bag->GetItemByPos(j))
                    {
                        if (!visitor.Visit(item))
                            return;
                    }
                }
            }
        }
    }
}

bool UseItemAction::UseItemAuto(PlayerbotAI* botAI, Item* item, bool allowCombat)
{
    if (!botAI || !item)
        return false;

    UseItemAction action(botAI);
    return action.UseItemOnSelf(item, allowCombat);
}

Item* UseItemAction::FindFirstItemByName(std::string const& name) const
{
    if (name.empty())
        return nullptr;

    FindUsableNamedItemVisitor visitor(bot, name);
    IterateItems(bot, visitor, ITERATE_ITEMS_IN_BAGS);

    std::vector<Item*>& items = visitor.GetResult();
    if (items.empty())
        return nullptr;

    return items.front();
}

bool UseItemAction::UseItemOnSelf(Item* item, bool allowCombat)
{
    if (!item)
        return false;

    if (bot->CanUseItem(item) != EQUIP_ERR_OK)
        return false;

    if (!allowCombat && bot->IsInCombat())
        return false;

    if (bot->IsNonMeleeSpellCasted(false))
        return false;

    if (bot->isMoving())
    {
        bot->StopMoving();
        botAI->SetNextCheckDelay(sPlayerbotAIConfig->globalCoolDown);
        return false;
    }

    uint8 bagIndex = item->GetBagSlot();
    uint8 slot = item->GetSlot();
    uint8 cast_count = 1;
    ObjectGuid item_guid = item->GetGUID();
    uint32 glyphIndex = 0;
    uint8 castFlags = 0;
    uint32 targetFlag = TARGET_FLAG_NONE;

    uint32 spellId = 0;
    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        if (item->GetTemplate()->Spells[i].SpellId > 0 &&
            item->GetTemplate()->Spells[i].SpellTrigger == ITEM_SPELLTRIGGER_ON_USE)
        {
            spellId = item->GetTemplate()->Spells[i].SpellId;
            if (!botAI->CanCastSpell(spellId, bot, false))
                return false;
            break;
        }
    }

    if (!spellId)
        return false;

    WorldPacket packet(CMSG_USE_ITEM);
    packet << bagIndex << slot << cast_count << spellId << item_guid << glyphIndex << castFlags;
    packet << targetFlag << bot->GetPackGUID();

    bot->GetSession()->HandleUseItemOpcode(packet);
    return true;
}

bool UseItemAction::Execute(Event event)
{
    std::string name = event.getParam();
    if (name.empty())
        name = getName();

    Item* item = FindFirstItemByName(name);
    if (!item)
        return false;

    return UseItemOnSelf(item, true);
}

bool UseItemAction::isPossible()
{
    if (getName() == "use")
        return true;

    return FindFirstItemByName(getName()) != nullptr;
}

bool UseHealingPotion::Execute(Event /*event*/)
{
    FindPotionVisitor visitor(bot, SPELL_EFFECT_HEAL);
    IterateItems(bot, visitor, ITERATE_ITEMS_IN_BAGS);

    std::vector<Item*>& items = visitor.GetResult();
    if (items.empty())
        return false;

    return UseItemOnSelf(items.front(), true);
}

bool UseHealingPotion::isUseful()
{
    return bot->IsInCombat();
}

bool UseManaPotion::Execute(Event /*event*/)
{
    FindPotionVisitor visitor(bot, SPELL_EFFECT_ENERGIZE);
    IterateItems(bot, visitor, ITERATE_ITEMS_IN_BAGS);

    std::vector<Item*>& items = visitor.GetResult();
    if (items.empty())
        return false;

    return UseItemOnSelf(items.front(), true);
}

bool UseManaPotion::isUseful()
{
    if (bot->GetPowerType() != POWER_MANA)
        return false;

    return bot->IsInCombat();
}

