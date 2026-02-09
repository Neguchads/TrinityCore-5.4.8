#include "TravelAction.h"

#include "Playerbots.h"
#include "TravelMgr.h"

bool TravelAction::Execute(Event /*event*/)
{
    TravelTarget* target = AI_VALUE(TravelTarget*, "travel target");
    if (!target)
        return false;

    target->setStatus(TRAVEL_STATUS_WORK);
    return true;
}

bool TravelAction::isUseful()
{
    return false;
}
