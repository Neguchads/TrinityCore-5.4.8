#include "RtiTriggers.h"

#include "Playerbots.h"

bool NoRtiTrigger::IsActive()
{
    if (!bot->IsInCombat())
        return false;

    Unit* target = AI_VALUE(Unit*, "rti target");
    return target == nullptr;
}
