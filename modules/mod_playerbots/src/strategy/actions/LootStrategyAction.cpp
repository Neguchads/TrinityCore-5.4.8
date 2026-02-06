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

#include "LootStrategyAction.h"

#include <cctype>
#include <set>
#include <sstream>

#include "ChatHelper.h"
#include "Event.h"
#include "ItemPrototype.h"
#include "LootAction.h"
#include "LootObjectStack.h"
#include "LootStrategyValue.h"
#include "ObjectMgr.h"
#include "Playerbots.h"

namespace
{
    std::set<uint32> ParseItemIds(std::string const& text)
    {
        std::set<uint32> items;

        size_t pos = text.find("Hitem:");
        while (pos != std::string::npos)
        {
            pos += 6;
            size_t end = pos;
            while (end < text.size() && std::isdigit(static_cast<unsigned char>(text[end])))
                ++end;
            if (end > pos)
                items.insert(uint32(atoi(text.substr(pos, end - pos).c_str())));
            pos = text.find("Hitem:", end);
        }

        bool hasDigits = false;
        uint32 num = 0;
        for (char c : text)
        {
            if (std::isdigit(static_cast<unsigned char>(c)))
            {
                hasDigits = true;
                num = num * 10 + uint32(c - '0');
            }
            else if (hasDigits)
            {
                items.insert(num);
                hasDigits = false;
                num = 0;
            }
        }
        if (hasDigits)
            items.insert(num);

        return items;
    }
}

bool LootStrategyAction::Execute(Event event)
{
    std::string const strategy = event.getParam();

    std::set<uint32>& alwaysLootItems = AI_VALUE(std::set<uint32>&, "always loot list");
    Value<LootStrategy*>* lootStrategy = context->GetValue<LootStrategy*>("loot strategy");

    if (strategy == "?")
    {
        {
            std::ostringstream out;
            out << "Loot strategy: " << lootStrategy->Get()->GetName();
            botAI->TellMaster(out);
        }

        {
            std::ostringstream out;
            out << "Always loot items: ";

            for (uint32 itemId : alwaysLootItems)
            {
                ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
                if (!proto)
                    continue;

                out << ChatHelper::FormatItem(proto) << " ";
            }

            botAI->TellMaster(out);
        }
        return true;
    }

    std::set<uint32> items = ParseItemIds(strategy);
    if (items.empty())
    {
        lootStrategy->Set(LootStrategyValue::instance(strategy));

        std::ostringstream out;
        out << "Loot strategy set to " << lootStrategy->Get()->GetName();
        botAI->TellMaster(out);
        return true;
    }

    bool remove = strategy.size() > 1 && strategy.substr(0, 1) == "-";
    bool query = strategy.size() > 1 && strategy.substr(0, 1) == "?";
    for (uint32 itemid : items)
    {
        if (query)
        {
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemid))
            {
                std::ostringstream out;
                out << (StoreLootAction::IsLootAllowed(itemid, botAI) ? "Will loot " : "Won't loot ")
                    << ChatHelper::FormatItem(proto);
                botAI->TellMaster(out.str());
            }
        }
        else if (remove)
        {
            auto j = alwaysLootItems.find(itemid);
            if (j != alwaysLootItems.end())
                alwaysLootItems.erase(j);

            botAI->TellMaster("Item(s) removed from always loot list");
        }
        else
        {
            alwaysLootItems.insert(itemid);
            botAI->TellMaster("Item(s) added to always loot list");
        }
    }

    return true;
}
