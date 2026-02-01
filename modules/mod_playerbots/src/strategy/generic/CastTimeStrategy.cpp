#include "CastTimeStrategy.h"

#include <algorithm>

#include "GenericSpellActions.h"
#include "Playerbots.h"
#include "SpellMgr.h"

float CastTimeMultiplier::GetValue(Action* action)
{
    if (!action)
        return 1.0f;

    if (!action->GetTarget() || action->GetTarget() != AI_VALUE(Unit*, "current target"))
        return 1.0f;

    CastSpellAction* spellAction = dynamic_cast<CastSpellAction*>(action);
    if (!spellAction)
        return 1.0f;

    uint32 spellId = AI_VALUE2(uint32, "spell id", spellAction->getName());
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
        return 1.0f;

    if ((spellInfo->Targets & TARGET_FLAG_DEST_LOCATION) != 0 || (spellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION) != 0)
        return 1.0f;

    uint32 castTime = spellInfo->CalcCastTime(bot->GetLevel());

    if (spellInfo->IsChanneled())
    {
        int32 duration = spellInfo->GetDuration();
        duration = std::min(duration, 3000);
        if (duration > 0)
            castTime += duration;
    }

    Unit* target = action->GetTarget();
    if (!target || !target->IsAlive() || !target->IsInWorld())
        return 1.0f;

    float expectedDps = AI_VALUE(float, "estimated group dps");
    if (expectedDps <= 0.0f)
        return 1.0f;

    if (castTime > (1000.0f * target->GetHealth() / expectedDps))
        return 0.1f;

    return 1.0f;
}

void CastTimeStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new CastTimeMultiplier(botAI));
}
