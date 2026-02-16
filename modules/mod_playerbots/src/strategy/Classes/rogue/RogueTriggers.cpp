

#include "RogueTriggers.h"

#include "GenericTriggers.h"
#include "Playerbots.h"
#include "ServerFacade.h"

namespace
{
bool IsRoguePvpTarget(Player* bot, Unit* target)
{
    return target && target->IsAlive() && bot->IsValidAttackTarget(target) &&
           (bot->duel || bot->InBattleground() || bot->InArena() || target->IsPlayer());
}
}  // namespace

bool KidneyShotPvpTrigger::IsActive()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!IsRoguePvpTarget(bot, target))
        return false;

    if (target->HasUnitState(UNIT_STATE_STUNNED))
        return false;

    return AI_VALUE2(uint8, "combo points", "current target") >= 2;
}

bool StealthPvpOpenerTrigger::IsActive()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!IsRoguePvpTarget(bot, target))
        return false;

    if (!botAI->HasAura("stealth", bot))
        return false;

    return !target->HasUnitState(UNIT_STATE_STUNNED);
}
