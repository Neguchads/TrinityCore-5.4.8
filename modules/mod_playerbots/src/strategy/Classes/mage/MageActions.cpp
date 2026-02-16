

#include "MageActions.h"

#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"
#include "ServerFacade.h"
#include "SharedDefines.h"

Value<Unit*>* CastPolymorphAction::GetTargetValue() { return context->GetValue<Unit*>("cc target", getName()); }

bool CastFireballAction::isUseful()
{
    if (AI_VALUE2(bool, "moving", "self target"))
        return false;

    return CastSpellAction::isUseful();
}

bool CastScorchAction::isUseful()
{
    bool const moving = AI_VALUE2(bool, "moving", "self target");
    if (!moving)
    {
        Unit* target = AI_VALUE(Unit*, "current target");
        bool const underClosePressure =
            target && sServerFacade->IsDistanceLessOrEqualThan(
                          AI_VALUE2(float, "distance", GetTargetName()), sPlayerbotAIConfig->tooCloseDistance);
        if (!underClosePressure)
            return false;
    }

    return CastSpellAction::isUseful();
}

bool CastInfernoBlastAction::isUseful()
{
    // Fire's movement filler is scorch; keep inferno blast for movement windows and Heating Up conversion.
    if (!AI_VALUE2(bool, "moving", "self target") && !botAI->HasAura("heating up", bot))
        return false;

    return CastSpellAction::isUseful();
}

bool CastFrostNovaAction::isUseful()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return sServerFacade->IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", GetTargetName()), 10.f);
}

bool CastBlinkAction::isUseful()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return sServerFacade->IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", GetTargetName()), 5.0f);
}

bool CastSlowAction::isUseful()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return sServerFacade->IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", GetTargetName()), 25.0f);
}
bool CastDeepFreezeAction::isUseful()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target) return false;

    return !botAI->HasAura("deep freeze", target);
}
bool CastConeOfColdAction::isUseful()
{
    bool facingTarget = AI_VALUE2(bool, "facing", "current target");
    bool targetClose = sServerFacade->IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", GetTargetName()), 10.f);
    return facingTarget && targetClose;
}

bool CastFrozenOrbAction::isUseful()
{
    bool facingTarget = AI_VALUE2(bool, "facing", "current target");
    bool targetClose = sServerFacade->IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", GetTargetName()), 10.f);
    return facingTarget && targetClose;
}

bool CastDragonsBreathAction::isUseful()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;
    bool facingTarget = AI_VALUE2(bool, "facing", "current target");
    bool targetClose = bot->IsWithinCombatRange(target, 10.0f);
    return facingTarget && targetClose;
}

bool CastBlastWaveAction::isUseful()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;
    bool targetClose = bot->IsWithinCombatRange(target, 10.0f);
    return targetClose;
}

Unit* CastFocusMagicOnPartyAction::GetTarget()
{
    Group* group = bot->GetGroup();
    if (!group)
        return nullptr;

    Unit* casterDps = nullptr;
    Unit* healer = nullptr;
    Unit* target = nullptr;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || member == bot || !member->IsAlive())
            continue;
        
        if (member->GetMap() != bot->GetMap() || bot->GetDistance(member) > sPlayerbotAIConfig->spellDistance)
            continue;

        if (member->HasAura(54646))
            continue;

        if (member->GetClass() == CLASS_MAGE)
            return member;

        if (!casterDps && PlayerBotSpec::IsCaster(member) && PlayerBotSpec::IsDps(member))
            casterDps = member;
        
        if (!healer && PlayerBotSpec::IsHeal(member))
            healer = member;

        if (!target)
            target = member;
    }
    
    if (casterDps)
        return casterDps;

    if (healer)
        return healer;

    return target;
}

bool CastRuneOfPowerAction::Execute(Event event)
{
    return botAI->CastSpell(spell, bot);
}
