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

#include "LootTriggers.h"

#include "LootObjectStack.h"
#include "Playerbots.h"
#include "ServerFacade.h"

bool LootAvailableTrigger::IsActive()
{
    bool distanceCheck = false;
    if (botAI->HasStrategy("stay", BOT_STATE_NON_COMBAT))
    {
        distanceCheck = sServerFacade->IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", "loot target"), CONTACT_DISTANCE);
    }
    else
    {
        distanceCheck = sServerFacade->IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", "loot target"),
            INTERACTION_DISTANCE - 2.0f);
    }

    return AI_VALUE(bool, "has available loot") &&
        (distanceCheck || AI_VALUE(GuidVector, "all targets").empty());
}

bool FarFromCurrentLootTrigger::IsActive()
{
    LootObject loot = AI_VALUE(LootObject, "loot target");
    if (!loot.IsLootPossible(bot))
        return false;

    return AI_VALUE2(float, "distance", "loot target") >= INTERACTION_DISTANCE - 2.0f;
}

bool CanLootTrigger::IsActive() { return AI_VALUE(bool, "can loot"); }
