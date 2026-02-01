

#include "PaladinActions.h"

#include "AiFactory.h"
#include "Event.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "SharedDefines.h"


bool CastSealSpellAction::isUseful() { return AI_VALUE2(bool, "combat", "self target"); }

Value<Unit*>* CastTurnUndeadAction::GetTargetValue() { return context->GetValue<Unit*>("cc target", getName()); }

Unit* CastRighteousDefenseAction::GetTarget()
{
    Unit* current_target = AI_VALUE(Unit*, "current target");
    if (!current_target)
    {
        return NULL;
    }
    return current_target->GetVictim();
}


bool CastDivineSacrificeAction::isUseful()
{
    return GetTarget() && (GetTarget() != nullptr) && CastSpellAction::isUseful() &&
           !botAI->HasAura("divine guardian", GetTarget(), false, false, -1, true);
}

bool CastCancelDivineSacrificeAction::Execute(Event event)
{
    botAI->RemoveAura("divine sacrifice");
    return true;
}

bool CastCancelDivineSacrificeAction::isUseful()
{
    return botAI->HasAura("divine sacrifice", GetTarget(), false, true, -1, true);
}

bool CastCrusaderStrikeAction::isUseful()
{
    Unit* target = GetTarget();
    if (!target) return false;

    return !botAI->HasAura("weakened blows", GetTarget(), false, true, -1, false) || bot->GetPower(Powers::POWER_HOLY_POWER) < 3;
}

bool CastWordOfGloryAction::isUseful()
{
    return bot->GetPower(Powers::POWER_HOLY_POWER) >= 3 && CastHealingSpellAction::isUseful();
}

bool CastWordOfGloryOnPartyAction::isUseful()
{
    return bot->GetPower(Powers::POWER_HOLY_POWER) >= 3 && HealPartyMemberAction::isUseful();
}

bool CastEternalFlameAction::isUseful()
{
    return bot->GetPower(Powers::POWER_HOLY_POWER) >= 3 && CastHealingSpellAction::isUseful();
}

bool CastEternalFlameOnPartyAction::isUseful()
{
    return bot->GetPower(Powers::POWER_HOLY_POWER) >= 3 && HealPartyMemberAction::isUseful();
}

bool CastLightOfDawnAction::isUseful()
{
    return bot->GetPower(Powers::POWER_HOLY_POWER) >= 3 && CastHealingSpellAction::isUseful();
}

bool CastLightOfDawnOnPartyAction::isUseful()
{
    return bot->GetPower(Powers::POWER_HOLY_POWER) >= 3 && HealPartyMemberAction::isUseful();
}

bool CastInquisitionAction::isUseful()
{
    if (bot->GetPower(Powers::POWER_HOLY_POWER) < 1)
        return false;

    return !botAI->HasAura("inquisition", bot) && CastBuffSpellAction::isUseful();
}

bool CastTemplarsVerdictAction::isUseful()
{
    return bot->GetPower(Powers::POWER_HOLY_POWER) >= 3 && CastMeleeSpellAction::isUseful();
}
