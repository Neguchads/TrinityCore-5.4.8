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

#include "LootStrategyValue.h"

#include "AiObjectContext.h"
#include "ItemPrototype.h"
#include "LootObjectStack.h"
#include "Playerbots.h"

class NormalLootStrategy : public LootStrategy
{
public:
    bool CanLoot(ItemTemplate const* proto, AiObjectContext* /*context*/) override
    {
        if (!proto)
            return false;

        if (proto->StartQuest || proto->Class == ITEM_CLASS_QUEST)
            return true;

        return proto->Quality > ITEM_QUALITY_POOR;
    }

    std::string const GetName() override { return "normal"; }
};

class GrayLootStrategy : public NormalLootStrategy
{
public:
    bool CanLoot(ItemTemplate const* proto, AiObjectContext* context) override
    {
        return NormalLootStrategy::CanLoot(proto, context) || proto->Quality == ITEM_QUALITY_POOR;
    }

    std::string const GetName() override { return "gray"; }
};

class DisenchantLootStrategy : public NormalLootStrategy
{
public:
    bool CanLoot(ItemTemplate const* proto, AiObjectContext* context) override
    {
        if (NormalLootStrategy::CanLoot(proto, context))
            return true;

        return proto->Quality >= ITEM_QUALITY_UNCOMMON && proto->Bonding != BIND_ON_ACQUIRE &&
            (proto->Class == ITEM_CLASS_ARMOR || proto->Class == ITEM_CLASS_WEAPON);
    }

    std::string const GetName() override { return "disenchant"; }
};

class AllLootStrategy : public LootStrategy
{
public:
    bool CanLoot(ItemTemplate const* /*proto*/, AiObjectContext* /*context*/) override { return true; }

    std::string const GetName() override { return "all"; }
};

LootStrategyValue::~LootStrategyValue() {}

LootStrategy* LootStrategyValue::normal = new NormalLootStrategy();
LootStrategy* LootStrategyValue::gray = new GrayLootStrategy();
LootStrategy* LootStrategyValue::disenchant = new DisenchantLootStrategy();
LootStrategy* LootStrategyValue::all = new AllLootStrategy();

LootStrategy* LootStrategyValue::instance(std::string const strategy)
{
    if (strategy == "*" || strategy == "all")
        return all;

    if (strategy == "g" || strategy == "gray")
        return gray;

    if (strategy == "d" || strategy == "e" || strategy == "disenchant" || strategy == "enchant")
        return disenchant;

    return normal;
}

std::string const LootStrategyValue::Save() { return value ? value->GetName() : "?"; }

bool LootStrategyValue::Load(std::string const text)
{
    value = LootStrategyValue::instance(text);
    return true;
}
