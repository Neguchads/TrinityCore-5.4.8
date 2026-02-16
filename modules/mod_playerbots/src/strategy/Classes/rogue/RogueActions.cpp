

#include "RogueActions.h"

#include "Event.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "Playerbots.h"

bool CastStealthAction::isPossible()
{
    // do not use with WSG flag or EYE flag
    return !botAI->HasAura(23333, bot) && !botAI->HasAura(23335, bot) && !botAI->HasAura(34976, bot);
}

bool UnstealthAction::Execute(Event event)
{
    if (bot->duel && !bot->IsInCombat())
        return false;

    botAI->RemoveAura("stealth");
    // botAI->ChangeStrategy("+dps,-stealthed", BOT_STATE_COMBAT);

    return true;
}

bool CheckStealthAction::Execute(Event event)
{
    if (botAI->HasAura("stealth", bot))
    {
        // Keep dps strategy active so PvP openers/finishers continue to run while stealthed.
        botAI->ChangeStrategy("+dps,+stealthed", BOT_STATE_COMBAT);

        Unit* target = AI_VALUE(Unit*, "current target");
        if (!target)
            target = AI_VALUE(Unit*, "duel target");
        if (!target)
            target = AI_VALUE(Unit*, "enemy player target");

        if (target && target->IsAlive() && bot->IsValidAttackTarget(target))
        {
            context->GetValue<Unit*>("current target")->Set(target);
            bot->SetSelection(target->GetGUID());

            if (botAI->DoSpecificAction("cheap shot", event, true) ||
                botAI->DoSpecificAction("garrote", event, true) ||
                botAI->DoSpecificAction("ambush", event, true))
            {
                return true;
            }

            botAI->DoSpecificAction("reach melee", event, true);
        }
    }
    else
    {
        botAI->ChangeStrategy("+dps,-stealthed", BOT_STATE_COMBAT);
    }

    return true;
}

bool CastVanishAction::isUseful()
{
    // do not use with WSG flag or EYE flag
    return !botAI->HasAura(23333, bot) && !botAI->HasAura(23335, bot) && !botAI->HasAura(34976, bot);
}

bool CastTricksOfTheTradeOnMainTankAction::isUseful()
{
    return CastSpellAction::isUseful() && AI_VALUE2(float, "distance", GetTargetName()) < 20.0f;
}
