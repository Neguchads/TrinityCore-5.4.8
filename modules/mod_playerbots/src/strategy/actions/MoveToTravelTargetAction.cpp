#include "MoveToTravelTargetAction.h"

#include "Playerbots.h"
#include "TravelMgr.h"

#include <cmath>

bool MoveToTravelTargetAction::Execute(Event /*event*/)
{
    TravelTarget* target = AI_VALUE(TravelTarget*, "travel target");
    if (!target || !target->isTraveling())
        return false;

    WorldLocation location = *target->getPosition();

    float maxDistance = target->getDestination()->getRadiusMin();
    float angle = 2.0f * static_cast<float>(M_PI) * urand(0, 100) / 100.0f;

    float x = location.GetPositionX();
    float y = location.GetPositionY();
    float z = location.GetPositionZ();
    uint32 mapId = location.GetMapId();

    float mod = frand(50.0f, 100.0f) / 100.0f;
    x += cos(angle) * maxDistance * mod;
    y += sin(angle) * maxDistance * mod;

    bool canMove = false;
    if (bot->IsWithinLOS(x, y, z))
        canMove = MoveNear(mapId, x, y, z, 0.0f);
    else
        canMove = MoveTo(mapId, x, y, z, false, false);

    if (!canMove && !target->isForced())
    {
        target->incRetry(true);
        if (target->isMaxRetry(true))
            target->setStatus(TRAVEL_STATUS_COOLDOWN);
    }
    else
    {
        target->setRetry(true);
    }

    return canMove;
}

bool MoveToTravelTargetAction::isUseful()
{
    if (!botAI->AllowActivity(TRAVEL_ACTIVITY))
        return false;

    TravelTarget* target = AI_VALUE(TravelTarget*, "travel target");
    if (!target || !target->isTraveling())
        return false;

    if (bot->HasUnitState(UNIT_STATE_IN_FLIGHT) || bot->IsFlying())
        return false;

    if (bot->isMoving())
        return false;

    if (!AI_VALUE(bool, "can move around"))
        return false;

    return true;
}
