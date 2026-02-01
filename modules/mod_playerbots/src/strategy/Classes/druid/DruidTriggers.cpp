
#include "DruidTriggers.h"

#include "Playerbots.h"

bool FerociousBiteTimeTrigger::IsActive()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target || !target->IsAlive())
    {
        return false;
    }
    uint8 comboPoints = AI_VALUE2(uint8, "combo", "current target");
    if (comboPoints < 5)
    {
        return false;
    }
    return target->GetHealthPct() <= 25.0f;
}
