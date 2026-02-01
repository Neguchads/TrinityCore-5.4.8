

#include "WarlockActions.h"

#include "Event.h"
#include "Playerbots.h"

bool CastDrainSoulAction::isUseful()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    return AI_VALUE2(uint8, "health", "current target") < 20;
}

Value<Unit*>* CastBanishAction::GetTargetValue() { return context->GetValue<Unit*>("cc target", "banish"); }

bool CastBanishAction::Execute(Event event) { return botAI->CastSpell("banish", GetTarget()); }

Value<Unit*>* CastFearOnCcAction::GetTargetValue() { return context->GetValue<Unit*>("cc target", "fear"); }

bool CastFearOnCcAction::Execute(Event event) { return botAI->CastSpell("fear", GetTarget()); }

bool CastFearOnCcAction::isPossible() { return true; }

bool CastFearOnCcAction::isUseful() { return true; }

bool CastLifeTapAction::isUseful() { return AI_VALUE2(uint8, "health", "self target") > sPlayerbotAIConfig->lowHealth; }

Unit* CastSoulstoneAction::GetTarget()
{
    Unit* mainTank = AI_VALUE(Unit*, "main tank");
    if (mainTank && mainTank->IsAlive() && !botAI->HasAura("soulstone", mainTank))
        return mainTank;

    Player* master = GetMaster();
    if (master && master->IsAlive() && !botAI->HasAura("soulstone", master))
        return master;

    if (!botAI->HasAura("soulstone", bot))
        return bot;

    return nullptr;
}

bool CastSoulstoneAction::isUseful()
{
    Unit* target = GetTarget();
    if (!target)
        return false;

    return CastBuffSpellAction::isUseful();
}

bool CastHandOfGuldanAction::Execute(Event event)
{
	return botAI->CastSpell(105174, GetTarget());
}

bool CastChaosBoltAction::isUseful()
{
	return botAI->HasAura("fire and brimstone", bot) || bot->GetPower(Powers::POWER_BURNING_EMBERS) >= 10;
}
bool CastIncinerateAction::Execute(Event event)
{
	if (!isUseful() || !isPossible())
		return false;

	if (botAI->HasAura("fire and brimstone", bot))
		botAI->CastSpell(114654, GetTarget());
	else
		botAI->CastSpell(spell, GetTarget());

	return true;
}
bool CastImmolateAction::Execute(Event event)
{
	if (!isUseful() || !isPossible())
		return false;

	if (botAI->HasAura("fire and brimstone", bot))
		botAI->CastSpell(108686, GetTarget());
	else
		botAI->CastSpell(spell, GetTarget());

	return true;
}
bool CastConflagrateAction::Execute(Event event)
{
	if (!isUseful() || !isPossible())
		return false;

	if (botAI->HasAura("fire and brimstone", bot))
		botAI->CastSpell(108685, GetTarget());
	else
		botAI->CastSpell(spell, GetTarget());

	return true;
}
