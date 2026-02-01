#include "EnemyHealerTargetValue.h"

#include "Playerbots.h"
#include "ServerFacade.h"

Unit* EnemyHealerTargetValue::Calculate()
{
    std::string const spellName = qualifier;

    GuidVector attackers = botAI->GetAiObjectContext()->GetValue<GuidVector>("attackers")->Get();
    Unit* currentTarget = botAI->GetAiObjectContext()->GetValue<Unit*>("current target")->Get();
    for (ObjectGuid const guid : attackers)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || unit == currentTarget)
            continue;

        if (sServerFacade->GetDistance2d(bot, unit) > botAI->GetRange("spell"))
            continue;

        if (!botAI->IsInterruptableSpellCasting(unit, spellName))
            continue;

        Spell* currentSpell = unit->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if (currentSpell && currentSpell->m_spellInfo->IsPositive())
            return unit;

        currentSpell = unit->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (currentSpell && currentSpell->m_spellInfo->IsPositive())
            return unit;
    }

    return nullptr;
}
