#include "CollisionValue.h"

#include <limits>

#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "ServerFacade.h"

bool CollisionValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, qualifier);
    if (!target)
        return false;

    GuidVector const& nearby = AI_VALUE(GuidVector, "nearest friendly players");
    for (ObjectGuid const& guid : nearby)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || unit == target)
            continue;

        float dist = sServerFacade->GetDistance2d(target, unit);
        float minDist = unit->GetCombatReach() + sPlayerbotAIConfig->contactDistance;
        if (sServerFacade->IsDistanceLessThan(dist, minDist))
            return true;
    }

    return false;
}
