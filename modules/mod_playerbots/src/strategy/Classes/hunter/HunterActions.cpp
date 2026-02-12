

#include "HunterActions.h"

#include <array>

#include "Event.h"
#include "GenericSpellActions.h"
#include "PetDefines.h"
#include "Playerbots.h"

bool CastHuntersMarkAction::isUseful() { return CastDebuffSpellAction::isUseful(); }

bool CastViperStingAction::isUseful()
{
    return AI_VALUE2(uint8, "mana", "self target") < 50 && AI_VALUE2(uint8, "mana", "current target") >= 30;
}

bool CastAspectOfTheCheetahAction::isUseful()
{
    return !botAI->HasAnyAuraOf(GetTarget(), "aspect of the cheetah", "aspect of the pack", nullptr);
}

Value<Unit*>* CastFreezingTrap::GetTargetValue() { return context->GetValue<Unit*>("cc target", "freezing trap"); }

bool CastAutoShotAction::isUseful()
{
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    if (AI_VALUE(Unit*, "current target") && bot->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL) &&
        bot->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL)->m_targets.GetUnitTargetGUID() ==
            AI_VALUE(Unit*, "current target")->GetGUID())
    {
        return false;
    }
    return AI_VALUE(uint32, "active spell") != AI_VALUE2(uint32, "spell id", getName());
}

Value<Unit*>* CastScareBeastCcAction::GetTargetValue() { return context->GetValue<Unit*>("cc target", "scare beast"); }

bool CastScareBeastCcAction::Execute(Event event) { return botAI->CastSpell("scare beast", GetTarget()); }

bool CastWingClipAction::isUseful() { return CastSpellAction::isUseful() && !botAI->HasAura(spell, GetTarget()); }

NextAction** CastWingClipAction::getPrerequisites() { return nullptr; }

bool CastFocusFireAction::isUseful()
{
    Unit* pet = AI_VALUE(Unit*, "pet target");
    if (!pet)
        return false;

    Aura* aura = botAI->GetAura("frenzy", pet, false, true, 5);
    return aura && CastBuffSpellAction::isUseful();
}

uint32 CastCallPetAction::GetCallPetSpellId() const
{
    if (AI_VALUE(Unit*, "pet target") || bot->GetGuardianPet() || bot->GetPet())
        return 0;

    struct CallPetEntry
    {
        uint32 spellId;
        uint8 slot;
    };

    static std::array<CallPetEntry, 5> const callPetSpells = {
        CallPetEntry{ 883, PET_SLOT_ACTIVE_FIRST + 0 },   // Call Pet 1
        CallPetEntry{ 83242, PET_SLOT_ACTIVE_FIRST + 1 }, // Call Pet 2
        CallPetEntry{ 83243, PET_SLOT_ACTIVE_FIRST + 2 }, // Call Pet 3
        CallPetEntry{ 83244, PET_SLOT_ACTIVE_FIRST + 3 }, // Call Pet 4
        CallPetEntry{ 83245, PET_SLOT_ACTIVE_FIRST + 4 }  // Call Pet 5
    };

    for (CallPetEntry const& callPet : callPetSpells)
    {
        if (!bot->HasSpell(callPet.spellId))
            continue;

        if (!bot->GetPetIdBySlot(callPet.slot))
            continue;

        if (!botAI->CanCastSpell(callPet.spellId, bot))
            continue;

        return callPet.spellId;
    }

    return 0;
}

bool CastCallPetAction::Execute(Event event)
{
    (void)event;

    uint32 spellId = GetCallPetSpellId();
    if (!spellId)
        return false;

    return botAI->CastSpell(spellId, GetTarget());
}

bool CastCallPetAction::isPossible() { return GetCallPetSpellId() != 0; }

bool CastCallPetAction::isUseful()
{
    if (AI_VALUE2(bool, "mounted", "self target"))
        return false;

    return GetCallPetSpellId() != 0;
}
