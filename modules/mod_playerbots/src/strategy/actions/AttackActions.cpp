

#include "AttackActions.h"

#include "CreatureAI.h"
#include "Event.h"
#include "LastMovementValue.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"
#include "ServerFacade.h"
#include "SharedDefines.h"
#include "Unit.h"

bool AttackAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target)
        return false;

    if (!target->IsInWorld())
    {
        return false;
    }

    if (bot->GetClass() == CLASS_ROGUE && target->IsPlayer() && !bot->IsInCombat() && bot->IsValidAttackTarget(target))
    {
        context->GetValue<Unit*>("current target")->Set(target);
        bot->SetSelection(target->GetGUID());

        if (!botAI->HasAura("stealth", bot))
        {
            if (botAI->DoSpecificAction("stealth", event, true))
            {
                botAI->DoSpecificAction("check stealth", event, true);
                return true;
            }
        }
        else
        {
            if (botAI->DoSpecificAction("cheap shot", event, true) ||
                botAI->DoSpecificAction("garrote", event, true) ||
                botAI->DoSpecificAction("ambush", event, true))
            {
                return true;
            }
        }
    }

    return Attack(target);
}

bool AttackMyTargetAction::Execute(Event event)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    ObjectGuid guid = master->GetTarget();
    if (!guid)
    {
        if (verbose)
            botAI->TellError("You have no target");

        return false;
    }

    botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Set({ guid });

    Unit* target = botAI->GetUnit(guid);
    if (!target)
        return false;

    if (bot->GetClass() == CLASS_ROGUE && target->IsPlayer() && !bot->IsInCombat() && bot->IsValidAttackTarget(target))
    {
        context->GetValue<Unit*>("current target")->Set(target);
        bot->SetSelection(target->GetGUID());

        if (!botAI->HasAura("stealth", bot))
        {
            if (botAI->DoSpecificAction("stealth", event, true))
            {
                botAI->DoSpecificAction("check stealth", event, true);
                return true;
            }
        }
        else
        {
            if (botAI->DoSpecificAction("cheap shot", event, true) ||
                botAI->DoSpecificAction("garrote", event, true) ||
                botAI->DoSpecificAction("ambush", event, true))
            {
                return true;
            }
        }
    }

    bool result = Attack(target);
    if (result)
        context->GetValue<ObjectGuid>("pull target")->Set(guid);

    return result;
}

bool AttackAction::Attack(Unit* target, bool with_pet /*true*/)
{
    if (bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLIGHT_MOTION_TYPE ||
        bot->HasUnitState(UNIT_STATE_IN_FLIGHT))
    {
        if (verbose)
            botAI->TellError("I cannot attack in flight");

        return false;
    }

    if (!target)
    {
        if (verbose)
            botAI->TellError("I have no target");

        return false;
    }

    if (!target->IsInWorld())
    {
        return false;
    }

    if (!bot->IsValidAttackTarget(target))
    {
        if (verbose)
            botAI->TellError("I cannot attack an invalid target");

        return false;
    }

    std::ostringstream msg;
    msg << target->GetName();

    if (bot->IsFriendlyTo(target))
    {
        msg << " is friendly to me";
        if (verbose)
            botAI->TellError(msg.str());

        return false;
    }

    if (!bot->IsWithinLOSInMap(target))
    {
        // Reposition on LOS breaks (duels/pvp pillars/terrain) instead of idling until LOS magically returns.
        if (IsMovingAllowed(target))
        {
            if (bot->IsMounted())
                bot->Dismount();

            float chaseDistance = PlayerBotSpec::IsMelee(bot) ? 0.0f : sPlayerbotAIConfig->tooCloseDistance;
            if (ChaseTo(target, chaseDistance, bot->GetAngle(target)))
                return true;
        }

        msg << " is not in my sight";
        if (verbose)
            botAI->TellError(msg.str());

        return false;
    }

    if (target->isDead())
    {
        msg << " is dead";
        if (verbose)
            botAI->TellError(msg.str());

        return false;
    }

    if (sPlayerbotAIConfig->IsInPvpProhibitedZone(bot->GetZoneId())
        && (target->IsPlayer() || target->IsPet()))
    {
        if (verbose)
            botAI->TellError("I cannot attack others in PvP prohibited zones");

        return false;
    }

    if (bot->IsMounted())
        bot->Dismount();

    ObjectGuid guid = target->GetGUID();
    bot->SetSelection(target->GetGUID());

    Unit* oldTarget = context->GetValue<Unit*>("current target")->Get();
    bool melee = bot->IsWithinMeleeRange(target) || PlayerBotSpec::IsMelee(bot);

    if (oldTarget == target && botAI->GetState() == BOT_STATE_COMBAT && bot->GetVictim() == target && (bot->HasUnitState(UNIT_STATE_MELEE_ATTACKING) == melee))
        return false;

    context->GetValue<Unit*>("old target")->Set(oldTarget);

    context->GetValue<Unit*>("current target")->Set(target);
    //context->GetValue<LootObjectStack*>("available loot")->Get()->Add(guid);

    LastMovement& lastMovement = AI_VALUE(LastMovement&, "last movement");
    if (lastMovement.priority < MovementPriority::MOVEMENT_COMBAT && bot->isMoving())
    {
        AI_VALUE(LastMovement&, "last movement").clear();
        bot->GetMotionMaster()->Clear(false);
        bot->StopMoving();
    }


    if (IsMovingAllowed() && !bot->HasInArc(CAST_ANGLE_IN_FRONT, target))
    {
        sServerFacade->SetFacingTo(bot, target);
    }
    botAI->ChangeEngine(BOT_STATE_COMBAT);

    bot->Attack(target, melee);
    return true;
}

bool AttackDuelOpponentAction::isUseful()
{
    return AI_VALUE(Unit*, "duel target");
}

bool AttackDuelOpponentAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "duel target");
    if (!target)
        return false;

    // Rogues should prep/open duels like PvP, not immediately auto-attack from non-stealth.
    if (bot->GetClass() == CLASS_ROGUE && !bot->IsInCombat() && bot->IsValidAttackTarget(target))
    {
        context->GetValue<Unit*>("current target")->Set(target);
        bot->SetSelection(target->GetGUID());

        if (!botAI->HasAura("stealth", bot))
        {
            if (botAI->DoSpecificAction("stealth", event, true))
            {
                botAI->DoSpecificAction("check stealth", event, true);
                return true;
            }
        }
        else
        {
            if (botAI->DoSpecificAction("cheap shot", event, true) ||
                botAI->DoSpecificAction("garrote", event, true) ||
                botAI->DoSpecificAction("ambush", event, true))
            {
                return true;
            }
        }
    }

    return Attack(target);
}
