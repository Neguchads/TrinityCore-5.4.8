#include "TravelTriggers.h"

#include "Playerbots.h"
#include "TravelMgr.h"

bool NoTravelTargetTrigger::IsActive()
{
    return !context->GetValue<TravelTarget*>("travel target")->Get()->isActive();
}

bool FarFromTravelTargetTrigger::IsActive()
{
    return context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling();
}
