/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license,
 * you may redistribute it and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "NearestGameObjects.h"

#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Playerbots.h"

GuidVector NearestGameObjects::Calculate()
{
    std::list<GameObject*> targets;
    AnyGameObjectInObjectRangeCheck u_check(bot, range);
    Trinity::GameObjectListSearcher<AnyGameObjectInObjectRangeCheck> searcher(bot, targets, u_check);
    Cell::VisitAllObjects(bot, searcher, range);

    GuidVector result;
    for (GameObject* go : targets)
    {
        if (!ignoreLos && !bot->IsWithinLOSInMap(go))
            continue;

        result.push_back(go->GetGUID());
    }

    return result;
}
