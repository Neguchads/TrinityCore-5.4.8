

#include "IsMovingValue.h"

#include "Playerbots.h"
#include "Map.h"

bool IsMovingValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, qualifier);

    if (!target)
        return false;

    return target->isMoving();
}

bool IsSwimmingValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, qualifier);

    if (!target)
        return false;

    return target->IsUnderWater() || (target->GetLiquidStatus() == LIQUID_MAP_IN_WATER && target->CanSwim());
}
