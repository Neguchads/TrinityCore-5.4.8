

#include "PriestTriggers.h"

#include "Playerbots.h"

bool ShadowOrbsTrigger::IsActive()
{
    return bot->GetPower(Powers::POWER_SHADOW_ORBS) >= minOrbs;
}
