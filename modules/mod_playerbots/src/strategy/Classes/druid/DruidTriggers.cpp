
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

bool EntanglingRootsKiteTrigger::IsActive()
{
    if (!DebuffTrigger::IsActive())
        return false;

    Unit* target = GetTarget();
    if (!target || target->GetMaxPower(POWER_MANA) == 0)
        return false;

    if (AI_VALUE(uint8, "attacker count") > 3)
        return false;

    GuidVector attackers = context->GetValue<GuidVector>("attackers")->Get();
    for (ObjectGuid const guid : attackers)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        if (botAI->HasAura("entangling roots", unit, false, true))
            return false;
    }

    return true;
}
