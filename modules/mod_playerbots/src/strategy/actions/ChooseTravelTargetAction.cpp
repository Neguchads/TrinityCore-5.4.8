#include "ChooseTravelTargetAction.h"

#include "Playerbots.h"
#include "TravelMgr.h"

#include <limits>

bool ChooseTravelTargetAction::Execute(Event /*event*/)
{
    TravelTarget* oldTarget = context->GetValue<TravelTarget*>("travel target")->Get();
    if (!oldTarget)
        return false;

    TravelTarget newTarget(botAI);

    if (oldTarget->isForced() && oldTarget->isActive())
        newTarget.copyTarget(oldTarget);
    else
        selectTarget(&newTarget, oldTarget);

    if (!newTarget.isActive() && !newTarget.isForced())
        return false;

    setNewTarget(&newTarget, oldTarget);
    return true;
}

void ChooseTravelTargetAction::selectTarget(TravelTarget* newTarget, TravelTarget* oldTarget)
{
    if (!newTarget || !oldTarget)
        return;

    // Prefer quest destinations when available.
    if (SetTargetFromDestinations(newTarget, TravelMgr::instance().getQuestTravelDestinations(bot)))
        return;

    // Visit RPG targets (towns, NPCs, vendors, etc.)
    if (SetTargetFromDestinations(newTarget, TravelMgr::instance().getRpgTravelDestinations(bot)))
        return;

    // Explore if enabled.
    if (botAI->HasStrategy("explore", BOT_STATE_NON_COMBAT))
        if (SetTargetFromDestinations(newTarget, TravelMgr::instance().getExploreTravelDestinations(bot)))
            return;

    // Fall back to grinding.
    if (SetTargetFromDestinations(newTarget, TravelMgr::instance().getGrindTravelDestinations(bot)))
        return;

    SetNullTarget(newTarget);
}

bool ChooseTravelTargetAction::SetTargetFromDestinations(TravelTarget* newTarget,
                                                         std::vector<TravelDestination*> const& dests)
{
    if (!newTarget || dests.empty())
        return false;

    WorldPosition botPos(bot);

    TravelDestination* bestDest = nullptr;
    float bestDist = std::numeric_limits<float>::max();

    for (TravelDestination* dest : dests)
    {
        if (!dest)
            continue;

        float dist = dest->distanceTo(&botPos);
        if (dist < bestDist)
        {
            bestDist = dist;
            bestDest = dest;
        }
    }

    if (!bestDest)
        return false;

    std::vector<WorldPosition*> points = bestDest->nextPoint(&botPos, true);
    if (points.empty())
        return false;

    newTarget->setTarget(bestDest, points.front());
    return true;
}

void ChooseTravelTargetAction::SetNullTarget(TravelTarget* newTarget)
{
    if (!newTarget)
        return;

    newTarget->setTarget(TravelMgr::instance().nullTravelDestination, TravelMgr::instance().nullWorldPosition, true);
}

void ChooseTravelTargetAction::setNewTarget(TravelTarget* newTarget, TravelTarget* oldTarget)
{
    if (!newTarget || !oldTarget)
        return;

    reportTarget(newTarget, oldTarget);

    // Apply new target
    oldTarget->copyTarget(newTarget);

    if (botAI->GetMaster() && oldTarget->isActive() &&
        oldTarget->getDestination()->getName() == "NullTravelDestination")
        oldTarget->setExpireIn(10 * IN_MILLISECONDS);

    RESET_AI_VALUE(GuidPosition, "rpg target");
    RESET_AI_VALUE(ObjectGuid, "pull target");
}

void ChooseTravelTargetAction::reportTarget(TravelTarget* newTarget, TravelTarget* oldTarget)
{
    if (!botAI->GetMaster())
        return;

    TravelDestination* destination = newTarget->getDestination();
    TravelDestination* oldDestination = oldTarget->getDestination();

    if (!destination || !oldDestination || destination == oldDestination)
        return;

    std::ostringstream out;
    out << "Traveling to " << destination->getTitle();
    botAI->TellMasterNoFacing(out.str());
}
