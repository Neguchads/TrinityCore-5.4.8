#include "SnareTargetValue.h"

#include "PlayerbotSpec.h"
#include "Playerbots.h"
#include "ServerFacade.h"

Unit* SnareTargetValue::Calculate()
{
    GuidVector attackers = botAI->GetAiObjectContext()->GetValue<GuidVector>("attackers")->Get();
    for (ObjectGuid const guid : attackers)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit)
            continue;

        if (bot->GetDistance(unit) > botAI->GetRange("spell"))
            continue;

        Unit* chaseTarget;
        switch (unit->GetMotionMaster()->GetCurrentMovementGeneratorType())
        {
            case FLEEING_MOTION_TYPE:
                return unit;
            case CHASE_MOTION_TYPE:
            {
                chaseTarget = sServerFacade->GetChaseTarget(unit);
                if (!chaseTarget)
                    continue;

                Player* chaseTargetPlayer = ObjectAccessor::FindPlayer(chaseTarget->GetGUID());
                bool shouldSnare = true;

                if (chaseTargetPlayer && !PlayerBotSpec::IsRanged(bot) && chaseTargetPlayer == bot)
                    shouldSnare = false;

                if (!unit->isMoving())
                    shouldSnare = false;

                if (unit->HasAuraType(SPELL_AURA_MOD_ROOT))
                    shouldSnare = false;

                if (chaseTargetPlayer && shouldSnare && !PlayerBotSpec::IsTank(chaseTargetPlayer))
                {
                    return unit;
                }
                break;
            }
            default:
                break;
        }
    }

    return nullptr;
}
