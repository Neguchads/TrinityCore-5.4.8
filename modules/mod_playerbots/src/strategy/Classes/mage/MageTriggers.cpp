

#include "MageTriggers.h"

#include "MageActions.h"
#include "Playerbots.h"

bool ArcaneBrillianceTrigger::IsActive()
{
    return BuffTrigger::IsActive() && !botAI->HasAura("arcane brilliance", GetTarget());
}

bool RuneOfPowerTrigger::IsActive()
{
    return BuffTrigger::IsActive() && !botAI->HasAura("rune of power", GetTarget());
}

bool TimeWarpTrigger::IsActive()
{
    Player* bot = botAI->GetBot();
    if (!bot || !bot->IsInCombat() || !bot->GetGroup())
    {
        return false;
    }
    if (botAI->HasAura("sated", bot) || botAI->HasAura("temporal displacement", bot) || botAI->HasAura("exhaustion", bot))
    {
        return false;
    }
    return BoostTrigger::IsActive();
}

bool MageArmorTrigger::IsActive()
{
    return 
        (botAI->GetBot()->GetSpecialization() != Specializations::SPEC_MAGE_FIRE && bot->GetGroup() ||
        botAI->GetBot()->GetSpecialization() == Specializations::SPEC_MAGE_ARCANE) &&
        !botAI->HasAura("mage armor", bot);
}

bool FrostArmorTrigger::IsActive()
{
    return (botAI->GetBot()->GetSpecialization() == Specializations::SPEC_MAGE_FROST && !botAI->HasAura("frost armor", bot) && !bot->GetGroup());
}

bool MoltenArmorTrigger::IsActive()
{
    return (botAI->GetBot()->GetSpecialization() == Specializations::SPEC_MAGE_FIRE && !botAI->HasAura("molten armor", bot));
}

bool FrostNovaOnTargetTrigger::IsActive()
{
    Unit* target = GetTarget();
    if (!target || !target->IsAlive() || !target->IsInWorld())
    {
        return false;
    }
    return botAI->HasAura(spell, target);
}

bool PolymorphReasonTrigger::IsActive()
{
    if (!HasCcTargetTrigger::IsActive())
        return false;

    Unit* ccTarget = AI_VALUE2(Unit*, "cc target", "polymorph");
    if (!ccTarget || !ccTarget->IsAlive() || !bot->IsValidAttackTarget(ccTarget))
        return false;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    bool underPressure =
        AI_VALUE(uint8, "my attacker count") >= 2 ||
        AI_VALUE2(uint8, "health", "self target") < sPlayerbotAIConfig->mediumHealth;

    // Use polymorph for control/peel, not as a default spam on the main kill target.
    if (!underPressure && currentTarget && ccTarget == currentTarget)
        return false;

    if (!underPressure && AI_VALUE(uint8, "attacker count") < 2)
        return false;

    return true;
}

bool IcyVeinsTrigger::IsActive()
{
    Unit* target = GetTarget();
    if (!target || !target->IsAlive() || !target->IsInWorld())
    {
        return false;
    }
    return BoostTrigger::IsActive() && !botAI->HasAura("icy veins", target);
}

bool FingersOfFrostSingleTrigger::IsActive()
{
    // Fingers of Frost "stack" count is always 1.
    // The value is instead stored in the charges.
    Aura* aura = botAI->GetAura("fingers of frost", bot, false, true, -1);
    return (aura && aura->GetCharges() == 1);
}

bool SlowNotOnTargetTrigger::IsActive()
{
    Unit* target = GetTarget();
    if (!target || !target->IsAlive() || !target->IsInWorld())
    {
        return false;
    }
    return !botAI->HasAura(spell, target);
}

bool HeatingUpTrigger::IsActive()
{
    Aura* aura = botAI->GetAura("heating up", bot, false, false, -1);
    return aura != nullptr;
}
