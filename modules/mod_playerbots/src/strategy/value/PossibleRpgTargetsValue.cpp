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

#include "PossibleRpgTargetsValue.h"

#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "ObjectGuid.h"
#include "Playerbots.h"
#include "ServerFacade.h"
#include "TravelMgr.h"

std::vector<uint32> PossibleRpgTargetsValue::allowedNpcFlags;

PossibleRpgTargetsValue::PossibleRpgTargetsValue(PlayerbotAI* botAI, float range)
    : NearestUnitsValue(botAI, "possible rpg targets", range, true)
{
    if (allowedNpcFlags.empty())
    {
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_INNKEEPER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_GOSSIP);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_QUESTGIVER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_FLIGHTMASTER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_BANKER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_GUILD_BANKER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_TRAINER_CLASS);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_TRAINER_PROFESSION);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR_AMMO);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR_FOOD);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR_POISON);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR_REAGENT);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_AUCTIONEER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_STABLEMASTER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_PETITIONER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_TABARDDESIGNER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_BATTLEMASTER);

        allowedNpcFlags.push_back(UNIT_NPC_FLAG_TRAINER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_REPAIR);
    }
}

void PossibleRpgTargetsValue::FindUnits(std::list<Unit*>& targets)
{
    Trinity::AnyUnitInObjectRangeCheck u_check(bot, range);
    Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher(bot, targets, u_check);
    Cell::VisitAllObjects(bot, searcher, range);
}

bool PossibleRpgTargetsValue::AcceptUnit(Unit* unit)
{
    if (!unit || !unit->IsInWorld() || unit->IsDuringRemoveFromWorld())
        return false;

    if (unit->IsHostileTo(bot) || unit->IsPlayer())
        return false;

    if (sServerFacade->GetDistance2d(bot, unit) <= sPlayerbotAIConfig->tooCloseDistance)
        return false;

    if (unit->HasNpcFlag(UNIT_NPC_FLAG_SPIRITHEALER))
        return false;

    for (uint32 npcFlag : allowedNpcFlags)
    {
        if (unit->HasNpcFlag(static_cast<NPCFlags>(npcFlag)))
            return true;
    }

    TravelTarget* travelTarget = context->GetValue<TravelTarget*>("travel target")->Get();
    if (travelTarget && travelTarget->getDestination() &&
        travelTarget->getDestination()->getEntry() == unit->GetEntry())
        return true;

    if (urand(1, 100) < 25 && unit->IsFriendlyTo(bot))
        return true;

    if (urand(1, 100) < 5)
        return true;

    return false;
}

std::vector<uint32> PossibleNewRpgTargetsValue::allowedNpcFlags;

PossibleNewRpgTargetsValue::PossibleNewRpgTargetsValue(PlayerbotAI* botAI, float range)
    : NearestUnitsValue(botAI, "possible new rpg targets", range, true)
{
    if (allowedNpcFlags.empty())
    {
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_INNKEEPER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_GOSSIP);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_QUESTGIVER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_FLIGHTMASTER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_BANKER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_GUILD_BANKER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_TRAINER_CLASS);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_TRAINER_PROFESSION);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR_AMMO);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR_FOOD);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR_POISON);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR_REAGENT);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_AUCTIONEER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_STABLEMASTER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_PETITIONER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_TABARDDESIGNER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_BATTLEMASTER);

        allowedNpcFlags.push_back(UNIT_NPC_FLAG_TRAINER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_REPAIR);
    }
}

GuidVector PossibleNewRpgTargetsValue::Calculate()
{
    std::list<Unit*> targets;
    FindUnits(targets);

    GuidVector results;
    std::vector<std::pair<ObjectGuid, float>> guidDistancePairs;
    for (Unit* unit : targets)
    {
        if (AcceptUnit(unit) && (ignoreLos || bot->IsWithinLOSInMap(unit)))
            guidDistancePairs.push_back({ unit->GetGUID(), bot->GetExactDist(unit) });
    }

    std::sort(guidDistancePairs.begin(), guidDistancePairs.end(),
        [](auto const& a, auto const& b) { return a.second < b.second; });

    for (auto const& pair : guidDistancePairs)
        results.push_back(pair.first);

    return results;
}

void PossibleNewRpgTargetsValue::FindUnits(std::list<Unit*>& targets)
{
    Trinity::AnyUnitInObjectRangeCheck u_check(bot, range);
    Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher(bot, targets, u_check);
    Cell::VisitAllObjects(bot, searcher, range);
}

bool PossibleNewRpgTargetsValue::AcceptUnit(Unit* unit)
{
    if (!unit || !unit->IsInWorld() || unit->IsDuringRemoveFromWorld())
        return false;

    if (unit->IsHostileTo(bot) || unit->IsPlayer())
        return false;

    if (unit->HasNpcFlag(UNIT_NPC_FLAG_SPIRITHEALER))
        return false;

    for (uint32 npcFlag : allowedNpcFlags)
    {
        if (unit->HasNpcFlag(static_cast<NPCFlags>(npcFlag)))
            return true;
    }

    return false;
}

std::vector<GameobjectTypes> PossibleNewRpgGameObjectsValue::allowedGOFlags;

GuidVector PossibleNewRpgGameObjectsValue::Calculate()
{
    std::list<GameObject*> targets;
    AnyGameObjectInObjectRangeCheck u_check(bot, range);
    Trinity::GameObjectListSearcher<AnyGameObjectInObjectRangeCheck> searcher(bot, targets, u_check);
    Cell::VisitAllObjects(bot, searcher, range);

    std::vector<std::pair<ObjectGuid, float>> guidDistancePairs;
    for (GameObject* go : targets)
    {
        bool flagCheck = false;
        for (uint32 goFlag : allowedGOFlags)
        {
            if (go->GetGoType() == goFlag)
            {
                flagCheck = true;
                break;
            }
        }
        if (!flagCheck)
            continue;

        if (!ignoreLos && !bot->IsWithinLOSInMap(go))
            continue;

        guidDistancePairs.push_back({ go->GetGUID(), bot->GetExactDist(go) });
    }

    GuidVector results;
    std::sort(guidDistancePairs.begin(), guidDistancePairs.end(),
        [](auto const& a, auto const& b) { return a.second < b.second; });

    for (auto const& pair : guidDistancePairs)
        results.push_back(pair.first);

    return results;
}
