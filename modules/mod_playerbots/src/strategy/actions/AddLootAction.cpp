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

#include "AddLootAction.h"

#include "AiObjectContext.h"
#include "Event.h"
#include "LootObjectStack.h"
#include "Playerbots.h"
#include "ServerFacade.h"

bool AddLootAction::Execute(Event event)
{
    ObjectGuid guid = event.getObject();
    if (!guid)
        return false;

    AiObjectContext* ctx = botAI->GetAiObjectContext();
    Value<LootObjectStack*>* lootValue = ctx ? ctx->GetValue<LootObjectStack*>("available loot") : nullptr;
    if (!lootValue)
        return false;

    LootObjectStack* lootStack = lootValue->Get();
    return lootStack ? lootStack->Add(guid) : false;
}

bool AddAllLootAction::Execute(Event /*event*/)
{
    bool added = false;

    AiObjectContext* ctx = botAI->GetAiObjectContext();
    Value<GuidVector>* gosValue = ctx ? ctx->GetValue<GuidVector>("nearest game objects") : nullptr;
    Value<GuidVector>* corpsesValue = ctx ? ctx->GetValue<GuidVector>("nearest corpses") : nullptr;

    if (!gosValue || !corpsesValue)
        return false;

    GuidVector gos = gosValue->Get();
    for (auto const& guid : gos)
        added |= AddLoot(guid);

    GuidVector corpses = corpsesValue->Get();
    for (auto const& guid : corpses)
        added |= AddLoot(guid);

    return added;
}

bool AddLootAction::isUseful() { return true; }

bool AddAllLootAction::isUseful() { return true; }

bool AddAllLootAction::AddLoot(ObjectGuid guid)
{
    AiObjectContext* ctx = botAI->GetAiObjectContext();
    Value<LootObjectStack*>* lootValue = ctx ? ctx->GetValue<LootObjectStack*>("available loot") : nullptr;
    if (!lootValue)
        return false;

    LootObjectStack* lootStack = lootValue->Get();
    return lootStack ? lootStack->Add(guid) : false;
}

bool AddGatheringLootAction::AddLoot(ObjectGuid guid)
{
    LootObject loot(bot, guid);

    WorldObject* wo = loot.GetWorldObject(bot);
    if (loot.IsEmpty() || !wo)
        return false;

    if (loot.skillId == SKILL_NONE)
        return false;

    if (!loot.IsLootPossible(bot))
        return false;

    return AddAllLootAction::AddLoot(guid);
}
