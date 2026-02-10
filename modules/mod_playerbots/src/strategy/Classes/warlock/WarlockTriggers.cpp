

#include "WarlockTriggers.h"

#include "GenericTriggers.h"
#include "Playerbots.h"

bool MetamorphosisTrigger::IsActive()
{
    if (botAI->HasAura("metamorphosis", bot) && bot->GetPower(Powers::POWER_DEMONIC_FURY) <= 50)
    {
        botAI->RemoveAura("metamorphosis");
    }
    return BoostTrigger::IsActive() && bot->GetPower(Powers::POWER_DEMONIC_FURY) >= 800;
}

bool CorruptionTrigger::IsActive()
{
    if (botAI->HasAura("metamorphosis", bot))
    {
        spell = "doom";
        return DebuffTrigger::IsActive();
    }

    spell = "corruption";
    return DebuffTrigger::IsActive() && !botAI->HasAura("doom", GetTarget(), false, true);
}

bool SoulstoneTrigger::IsActive()
{
    Unit* mainTank = AI_VALUE(Unit*, "main tank");
    if (mainTank && mainTank->IsAlive() && !botAI->HasAura("soulstone", mainTank))
        return botAI->CanCastSpell("soulstone", mainTank);

    Player* master = GetMaster();
    if (master && master->IsAlive() && !botAI->HasAura("soulstone", master))
        return botAI->CanCastSpell("soulstone", master);

    if (!botAI->HasAura("soulstone", bot))
        return botAI->CanCastSpell("soulstone", bot);

    return false;
}

bool LifeTapGlyphBuffTrigger::IsActive()
{
    return !botAI->HasAura("life tap", bot);
}

bool FireAndBrimeStoneTrigger::IsActive()
{
    if (botAI->HasAura("fire and brimstone", bot) && bot->GetPower(Powers::POWER_BURNING_EMBERS) <= 0)
    {
        botAI->RemoveAura("fire and brimstone");
    }
    return BuffTrigger::IsActive() && bot->GetPower(Powers::POWER_BURNING_EMBERS) >= 40;
}

bool WarlockPetLowHealthTrigger::IsActive()
{
    Unit* pet = AI_VALUE(Unit*, "pet target");
    return pet && AI_VALUE2(uint8, "health", "pet target") < 40 && !AI_VALUE2(bool, "dead", "pet target") /*&& !AI_VALUE2(bool, "mounted", "self target")*/;
}
bool WarlockPetMediumHealthTrigger::IsActive()
{
    Unit* pet = AI_VALUE(Unit*, "pet target");
    return pet && AI_VALUE2(uint8, "health", "pet target") < sPlayerbotAIConfig->mediumHealth /* && !AI_VALUE2(bool, "dead", "pet target") && !AI_VALUE2(bool, "mounted", "self target")*/;
}
