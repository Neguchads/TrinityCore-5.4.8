

#include "MovementActions.h"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <algorithm>
#include <limits>

#include "Event.h"
#include "PositionValue.h"
#include "G3D/Vector3.h"
#include "GameObject.h"
#include "Group.h"
#include "LastMovementValue.h"
#include "Map.h"
#include "MotionMaster.h"
#include "MoveSplineInitArgs.h"
#include "MovementGenerator.h"
#include "ObjectDefines.h"
#include "ObjectGuid.h"
#include "PathGenerator.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "LootObjectStack.h"
#include "PlayerbotSpec.h"
#include "Position.h"

#include "Random.h"
#include "ServerFacade.h"
#include "SharedDefines.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "Travel/TravelNode.h"
#include "Travel/WorldPosition.h"

#include "TargetedMovementGenerator.h"
#include "Timer.h"
#include "Transport.h"
#include "Unit.h"
#include "Vehicle.h"
#include "WaypointMovementGenerator.h"
#include "Corpse.h"
#include "Creature.h"

namespace
{
/*
 * TrinityCore can occasionally leave MOVEMENTFLAG_BACKWARD set after backpedal/reposition updates.
 * If a new forward chase/follow/point move starts while that stale flag is still active,
 * clients can render reverse-run jitter ("moonwalk"/forward-backward flicker).
 *
 * Clear the backward flag before issuing normal forward movement generators.
 */
void ClearBackwardMovementFlag(Unit* unit)
{
    if (!unit)
        return;

    if (unit->HasUnitMovementFlag(MOVEMENTFLAG_BACKWARD))
    {
        unit->RemoveUnitMovementFlag(MOVEMENTFLAG_BACKWARD);
        unit->SendMovementFlagUpdate();
    }
}
}

MovementAction::MovementAction(PlayerbotAI* botAI, std::string const name) : Action(botAI, name)
{
    bot = botAI->GetBot();
}

bool MovementAction::isPossible()
{
    return IsMovingAllowed();
}

void MovementAction::CreateWp(Player* wpOwner, float x, float y, float z, float o, uint32 entry, bool important)
{
    if (!wpOwner)
        return;

    float const dist = wpOwner->GetExactDist(x, y, z);
    float delay = 1000.0f * dist / wpOwner->GetSpeed(MOVE_RUN) + sPlayerbotAIConfig->reactDelay;
    if (delay < 500.0f)
        delay = 500.0f;

    Creature* wpCreature = wpOwner->SummonCreature(entry, x, y, z - 1.0f, o, TEMPSUMMON_TIMED_DESPAWN, uint32(delay));
    if (!wpCreature)
        return;

    wpCreature->AddAura(246, wpCreature);
    if (!important)
        wpCreature->SetObjectScale(0.5f);
}

void MovementAction::ClearIdleState()
{
    context->GetValue<time_t>("stay time")->Set(0);
    context->GetValue<PositionMap&>("position")->Get()["random"].Reset();
}

void MovementAction::WaitForReach(float distance)
{
    float delay = 1000.0f * MoveDelay(distance);

    if (delay > sPlayerbotAIConfig->maxWaitForMove)
        delay = sPlayerbotAIConfig->maxWaitForMove;

    Unit* target = *botAI->GetAiObjectContext()->GetValue<Unit*>("current target");
    Unit* player = *botAI->GetAiObjectContext()->GetValue<Unit*>("enemy player target");
    if ((player || target) && delay > sPlayerbotAIConfig->globalCoolDown)
        delay = sPlayerbotAIConfig->globalCoolDown;

    if (delay < 0)
        delay = 0;

    botAI->SetNextCheckDelay((uint32)delay);
}

void MovementAction::UpdateMovementState()
{
    auto botInLiquidState = bot->GetLiquidStatus();

    if ((botInLiquidState & LIQUID_MAP_IN_WATER) || (botInLiquidState & LIQUID_MAP_UNDER_WATER))
    {
        bot->SetSwim(true);
    }
    else
    {
        bot->SetSwim(false);
    }

    bool onGround = bot->GetPositionZ() < bot->GetMapWaterOrGroundLevel(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ()) + 1.0f;

    bool hasMountedFlightAura = bot->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ||
        bot->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS);

    // Keep bot->SendMovementFlagUpdate() withing the if statements to not intefere with bot behavior on ground/(shallow) waters
    if (!bot->HasUnitMovementFlag(MOVEMENTFLAG_FLYING) &&
        hasMountedFlightAura && !onGround)
    {
        bot->AddUnitMovementFlag(MOVEMENTFLAG_FLYING);
        bot->SendMovementFlagUpdate();
    }

    else if (bot->HasUnitMovementFlag(MOVEMENTFLAG_FLYING) &&
        (!hasMountedFlightAura || onGround))
    {
        bot->RemoveUnitMovementFlag(MOVEMENTFLAG_FLYING);
        bot->SendMovementFlagUpdate();
    }
}

bool MovementAction::ChaseTo(WorldObject* obj, float distance, float angle)
{
    if (!IsMovingAllowed())
    {
        return false;
    }

    if (Vehicle* vehicle = bot->GetVehicle())
    {
        VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(bot);
        if (!seat || !seat->CanControl())
            return false;

        // vehicle->GetMotionMaster()->Clear();
        vehicle->GetBase()->GetMotionMaster()->MoveChase((Unit*)obj, 30.0f);
        return true;
    }

    UpdateMovementState();

    if (!bot->IsStandState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        botAI->InterruptSpell();
    }

    Unit* chaseTarget = obj->ToUnit();
    if (!chaseTarget)
        return false;

    if (bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE && !bot->IsStopped())
    {
        Unit* currentChaseTarget = sServerFacade->GetChaseTarget(bot);
        if (currentChaseTarget && currentChaseTarget->GetGUID() == chaseTarget->GetGUID())
            return true;
    }

    ClearBackwardMovementFlag(bot);
    bot->GetMotionMaster()->MoveChase(chaseTarget, distance);

    // TODO shouldnt this use "last movement" value?
    WaitForReach(bot->GetExactDist2d(obj) - distance);
    return true;
}

bool MovementAction::ReachCombatTo(Unit* target, float distance)
{
    if (!IsMovingAllowed(target))
        return false;

    float bx = bot->GetPositionX();
    float by = bot->GetPositionY();
    float bz = bot->GetPositionZ();

    float tx = target->GetPositionX();
    float ty = target->GetPositionY();
    float tz = target->GetPositionZ();
    float combatDistance = bot->GetCombatReach() + target->GetCombatReach();
    distance += combatDistance;

    if (bot->GetExactDist(tx, ty, tz) <= distance)
        return false;

    PathGenerator path(bot);
    path.CalculatePath(tx, ty, tz, false);
    PathType type = path.GetPathType();
    int typeOk = PATHFIND_NORMAL | PATHFIND_INCOMPLETE | PATHFIND_SHORTCUT;
    if (!(type & typeOk))
        return false;
    float shortenTo = distance;

    // Avoid walking too far when moving towards each other
    float disToGo = bot->GetExactDist(tx, ty, tz) - distance;
    if (disToGo >= 10.0f)
        shortenTo = disToGo / 2 + distance;

    path.ShortenPathUntilDist(G3D::Vector3(tx, ty, tz), shortenTo);
    G3D::Vector3 endPos = path.GetPath().back();
    return MoveTo(target->GetMapId(), endPos.x, endPos.y, endPos.z, false, false, false, false,
        MovementPriority::MOVEMENT_COMBAT, true);
}

bool MovementAction::IsDuplicateMove(uint32 mapId, float x, float y, float z)
{
    LastMovement& lastMove = *context->GetValue<LastMovement&>("last movement");

    // heuristic 5s
    if (lastMove.msTime + sPlayerbotAIConfig->maxWaitForMove < getMSTime() ||
        lastMove.lastMoveShort.GetExactDist(x, y, z) > 0.01f)
        return false;

    return true;
}

bool MovementAction::IsWaitingForLastMove(MovementPriority priority)
{
    LastMovement& lastMove = *context->GetValue<LastMovement&>("last movement");

    if (priority > lastMove.priority)
        return false;

    // heuristic 5s
    if (lastMove.lastdelayTime + lastMove.msTime > getMSTime())
        return true;

    return false;
}

float MovementAction::MoveDelay(float distance, bool backwards)
{
    float speed;
    if (bot->isSwimming())
    {
        speed = backwards ? bot->GetSpeed(MOVE_SWIM_BACK) : bot->GetSpeed(MOVE_SWIM);
    }
    else if (bot->IsFlying())
    {
        speed = backwards ? bot->GetSpeed(MOVE_FLIGHT_BACK) : bot->GetSpeed(MOVE_FLIGHT);
    }
    else
    {
        speed = backwards ? bot->GetSpeed(MOVE_RUN_BACK) : bot->GetSpeed(MOVE_RUN);
    }
    float delay = distance / speed;
    return delay;
}

bool MovementAction::MoveTo(WorldObject* target, float distance, MovementPriority priority)
{
    if (!IsMovingAllowed(target))
        return false;

    float bx = bot->GetPositionX();
    float by = bot->GetPositionY();
    float bz = bot->GetPositionZ();

    float tx = target->GetPositionX();
    float ty = target->GetPositionY();
    float tz = target->GetPositionZ();

    float distanceToTarget = bot->GetDistance(target);
    float angle = bot->GetAngle(target);
    float needToGo = distanceToTarget - distance;

    float maxDistance = sPlayerbotAIConfig->spellDistance;
    if (needToGo > 0 && needToGo > maxDistance)
        needToGo = maxDistance;
    else if (needToGo < 0 && needToGo < -maxDistance)
        needToGo = -maxDistance;

    float dx = cos(angle) * needToGo + bx;
    float dy = sin(angle) * needToGo + by;
    float dz;  // = std::max(bz, tz); // calc accurate z position to avoid stuck
    if (distanceToTarget > CONTACT_DISTANCE)
    {
        dz = bz + (tz - bz) * (needToGo / distanceToTarget);
    }
    else
    {
        dz = tz;
    }
    return MoveTo(target->GetMapId(), dx, dy, dz, false, false, false, false, priority);
}

bool MovementAction::FlyDirect(WorldPosition& startPosition, WorldPosition& endPosition, WorldPosition& movePosition,
    TravelPath movePath, bool idle)
{
    if (!bot || !startPosition || !endPosition)
        return false;

    // Only handle direct flight movement while already airborne and on the same map.
    if (!bot->IsFlying() || bot->HasUnitState(UNIT_STATE_IN_FLIGHT))
        return false;

    if (startPosition.getMapId() != endPosition.getMapId())
        return false;

    float const minDist = std::max(1.0f, sPlayerbotAIConfig->targetPosRecalcDistance);
    float const maxDist = std::max(minDist, sPlayerbotAIConfig->reactDistance);

    float totalDistance = startPosition.fDist(endPosition);
    if (totalDistance < minDist)
        return false;

    movePosition = endPosition;

    if (!movePath.empty())
    {
        float bestDistance = 0.0f;
        std::vector<WorldPosition> const pathPoints = movePath.getPointPath();
        for (WorldPosition point : pathPoints)
        {
            if (!point || point.getMapId() != startPosition.getMapId())
                continue;

            if (!bot->IsWithinLOS(point.getX(), point.getY(), point.getZ()))
                continue;

            float const dist = startPosition.fDist(point);
            if (dist <= bestDistance)
                continue;

            bestDistance = dist;
            movePosition = point;
        }
    }

    float moveDistance = startPosition.fDist(movePosition);
    if (moveDistance > maxDist)
    {
        float const ratio = maxDist / moveDistance;
        float const nx = startPosition.getX() + (movePosition.getX() - startPosition.getX()) * ratio;
        float const ny = startPosition.getY() + (movePosition.getY() - startPosition.getY()) * ratio;
        float const nz = startPosition.getZ() + (movePosition.getZ() - startPosition.getZ()) * ratio;

        movePosition = WorldPosition(startPosition.getMapId(), nx, ny, nz, startPosition.getO());
        moveDistance = maxDist;
    }

    if (moveDistance < minDist)
        return false;

    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        botAI->InterruptSpell();
    }

    MotionMaster& mm = *bot->GetMotionMaster();
    mm.Clear();
    mm.MovePoint(0, movePosition.getX(), movePosition.getY(), movePosition.getZ(), false);

    float delay = 1000.0f * MoveDelay(moveDistance);
    delay = std::max(0.0f, delay);
    delay = std::min((float)sPlayerbotAIConfig->maxWaitForMove, delay);

    AI_VALUE(LastMovement&, "last movement")
        .Set(movePosition.getMapId(), movePosition.getX(), movePosition.getY(), movePosition.getZ(),
            startPosition.getAngleTo(movePosition), delay, MovementPriority::MOVEMENT_NORMAL);

    if (!idle)
        ClearIdleState();

    if (botAI->HasStrategy("debug move", BOT_STATE_NON_COMBAT))
        CreateWp(bot, movePosition.getX(), movePosition.getY(), movePosition.getZ(), 0.0f, 2334, true);

    return true;
}

bool MovementAction::MoveTo(uint32 mapId, float x, float y, float z, bool idle, bool react, bool normal_only,
    bool exact_waypoint, MovementPriority priority, bool lessDelay, bool backwards)
{
    UpdateMovementState();
    if (!IsMovingAllowed(mapId, x, y, z))
    {
        return false;
    }
    if (IsDuplicateMove(mapId, x, y, z))
    {
        return false;
    }
    if (IsWaitingForLastMove(priority))
    {
        return false;
    }
    bool generatePath = !bot->IsFlying() && !bot->IsUnderWater() && !bot->IsInWater();
    bool disableMoveSplinePath = sPlayerbotAIConfig->disableMoveSplinePath >= 2 ||
        (sPlayerbotAIConfig->disableMoveSplinePath == 1 && bot->InBattleground());

    WorldPosition startPosition(bot);
    WorldPosition endPosition(mapId, x, y, z, bot->GetOrientation());
    WorldPosition movePosition;
    if (!exact_waypoint && !disableMoveSplinePath && !backwards &&
        FlyDirect(startPosition, endPosition, movePosition, TravelPath(), idle))
    {
        return true;
    }

    if (Vehicle* vehicle = bot->GetVehicle())
    {
        VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(bot);
        Unit* vehicleBase = vehicle->GetBase();
        generatePath = vehicleBase->CanFly();
        if (!vehicleBase || !seat || !seat->CanControl())  // is passenger and cant move anyway
            return false;

        float distance = vehicleBase->GetExactDist(x, y, z);  // use vehicle distance, not bot
        if (distance > 0.01f)
        {
            MotionMaster& mm = *vehicleBase->GetMotionMaster();  // need to move vehicle, not bot
            mm.Clear();
            if (!backwards)
            {
                ClearBackwardMovementFlag(vehicleBase);
                mm.MovePoint(0, x, y, z, generatePath);
            }
            else
            {
                mm.MovePointBackwards(0, x, y, z, generatePath);
            }
            float speed = backwards ? vehicleBase->GetSpeed(MOVE_RUN_BACK) : vehicleBase->GetSpeed(MOVE_RUN);
            float delay = 1000.0f * (distance / speed);
            if (lessDelay)
            {
                delay -= botAI->GetReactDelay();
            }
            delay = std::max(.0f, delay);
            delay = std::min((float)sPlayerbotAIConfig->maxWaitForMove, delay);
            AI_VALUE(LastMovement&, "last movement").Set(mapId, x, y, z, bot->GetOrientation(), delay, priority);
            return true;
        }
    }
    else if (exact_waypoint || disableMoveSplinePath || !generatePath)
    {
        float distance = bot->GetExactDist(x, y, z);
        if (distance > 0.01f)
        {
            if (bot->IsSitState())
                bot->SetStandState(UNIT_STAND_STATE_STAND);

            MotionMaster& mm = *bot->GetMotionMaster();
            mm.Clear();
            if (!backwards)
            {
                ClearBackwardMovementFlag(bot);
                mm.MovePoint(0, x, y, z, generatePath);
            }
            else
            {
                mm.MovePointBackwards(0, x, y, z, generatePath);
            }
            float delay = 1000.0f * MoveDelay(distance, backwards);
            if (lessDelay)
            {
                delay -= botAI->GetReactDelay();
            }
            delay = std::max(.0f, delay);
            delay = std::min((float)sPlayerbotAIConfig->maxWaitForMove, delay);
            AI_VALUE(LastMovement&, "last movement").Set(mapId, x, y, z, bot->GetOrientation(), delay, priority);
            return true;
        }
    }
    else
    {
        float modifiedZ;
        Movement::PointsArray path = SearchForBestPath(x, y, z, modifiedZ, sPlayerbotAIConfig->maxMovementSearchTime, normal_only);
        if (modifiedZ == INVALID_HEIGHT)
        {
            return false;
        }
        float distance = bot->GetExactDist(x, y, modifiedZ);
        if (distance > 0.01f)
        {
            if (bot->IsSitState())
                bot->SetStandState(UNIT_STAND_STATE_STAND);

            if (generatePath)
            {
                WorldPosition endPosition(mapId, x, y, modifiedZ, bot->GetOrientation());
                Movement::PointsArray adjustedPath;
                if (GeneratePathAvoidingHazards(endPosition, true, adjustedPath) && !adjustedPath.empty())
                {
                    path = std::move(adjustedPath);
                    G3D::Vector3 const& adjustedEnd = path.back();
                    x = adjustedEnd.x;
                    y = adjustedEnd.y;
                    z = adjustedEnd.z;
                    modifiedZ = z;
                }
            }

            MotionMaster& mm = *bot->GetMotionMaster();
            if (!path.empty())
            {
                G3D::Vector3 const& endP = path.back();
                x = endP.x;
                y = endP.y;
                z = endP.z;
            }
            mm.Clear();
            if (!backwards)
            {
                ClearBackwardMovementFlag(bot);
                mm.MovePoint(0, x, y, z, generatePath);
            }
            else
            {
                mm.MovePointBackwards(0, x, y, z, generatePath);
            }
             float delay = 1000.0f * MoveDelay(distance, backwards);
            if (lessDelay)
            {
                delay -= botAI->GetReactDelay();
            }
            delay = std::max(.0f, delay);
            delay = std::min((float)sPlayerbotAIConfig->maxWaitForMove, delay);
            AI_VALUE(LastMovement&, "last movement").Set(mapId, x, y, z, bot->GetOrientation(), delay, priority);
            return true;
        }
    }
    return false;
}

bool MoveRandomAction::Execute(Event event)
{
    float distance = sPlayerbotAIConfig->tooCloseDistance + urand(10, 30);
    const float x = bot->GetPositionX();
    const float y = bot->GetPositionY();
    const float z = bot->GetPositionZ();
    int attempts = 5;
    Map* map = bot->GetMap();
    while (--attempts)
    {
        float angle = (float)rand_norm() * 2 * static_cast<float>(M_PI);
        float dx = x + distance * cos(angle);
        float dy = y + distance * sin(angle);
        float dz = z;
        if (!map->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(),
            dx, dy, dz))
            continue;

        if (map->IsInWater(bot->GetPhaseMask(), dx, dy, dz))
            continue;

        bool moved = MoveTo(bot->GetMapId(), dx, dy, dz, false, false, false, true);
        if (moved)
            return true;
    }

    return false;
}

bool MovementAction::MoveNear(uint32 mapId, float x, float y, float z, float distance, MovementPriority priority)
{
    float angle = GetFollowAngle();
    return MoveTo(mapId, x + cos(angle) * distance, y + sin(angle) * distance, z, false, false, false, false, priority);
}

bool MovementAction::MoveNear(WorldObject* target, float distance, MovementPriority priority)
{
    if (!target)
        return false;

    distance += target->GetCombatReach();

    float x = target->GetPositionX();
    float y = target->GetPositionY();
    float z = target->GetPositionZ();
    float followAngle = GetFollowAngle();

    for (float angle = followAngle; angle <= followAngle + static_cast<float>(2 * M_PI);
        angle += static_cast<float>(M_PI / 4.f))
    {
        float x = target->GetPositionX() + cos(angle) * distance;
        float y = target->GetPositionY() + sin(angle) * distance;
        float z = target->GetPositionZ();

        if (!bot->IsWithinLOS(x, y, z))
            continue;

        bool moved = MoveTo(target->GetMapId(), x, y, z, false, false, false, false, priority);
        if (moved)
            return true;
    }

    // botAI->TellError("All paths not in LOS");
    return false;
}

float MovementAction::GetFollowAngle()
{
    Player* master = GetMaster();
    Group* group = master ? master->GetGroup() : bot->GetGroup();
    if (!group)
        return 0.0f;

    uint32 index = 1;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        if (ref->GetSource() == master)
            continue;

        if (ref->GetSource() == bot)
            return 2 * M_PI / (group->GetMembersCount() - 1) * index;

        ++index;
    }

    return 0;
}

bool MovementAction::FollowOnTransport(Unit* target)
{
    if (!target)
        return false;

    Transport* myTransport = bot->GetTransport();
    Transport* targetTransport = target->GetTransport();
    if (myTransport == targetTransport || (!myTransport && !targetTransport))
        return false;

    if (sServerFacade->GetDistance2d(bot, target) > sPlayerbotAIConfig->sightDistance)
        return false;

    botAI->StopMoving();

    if (myTransport)
        myTransport->RemovePassenger(bot);

    bot->Relocate(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());

    if (targetTransport)
        targetTransport->AddPassenger(bot);

    return true;
}

bool MoveRandomAction::isUseful()
{
    return true;
}

bool MovementAction::IsMovingAllowed(WorldObject* target)
{
    if (!target)
        return false;

    if (bot->GetMapId() != target->GetMapId())
        return false;

    return IsMovingAllowed();
}

bool MovementAction::IsMovingAllowed(uint32 mapId, float x, float y, float z)
{
    return IsMovingAllowed();
}

bool MovementAction::IsMovingAllowed()
{
    // do not allow if not vehicle driver
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(true))
        return false;

    if (bot->isFrozen() || bot->IsPolymorphed() || (bot->isDead() && !bot->HasPlayerFlag(PLAYER_FLAGS_GHOST)) ||
        bot->IsBeingTeleported() || bot->HasRootAura() || bot->HasSpiritOfRedemptionAura() ||
        bot->HasConfuseAura() || bot->IsCharmed() || bot->HasStunAura() ||
        bot->IsInFlight() || bot->HasUnitState(UNIT_STATE_LOST_CONTROL))
        return false;

    if (bot->GetMotionMaster()->GetMotionSlotType(MOTION_SLOT_CONTROLLED) != NULL_MOTION_TYPE)
    {
        return false;
    }

    return bot->GetMotionMaster()->GetCurrentMovementGeneratorType() != FLIGHT_MOTION_TYPE;
}

bool MoveRandomAction::isPossible()
{
    if (bot->IsInCombat() ||
        !AI_VALUE(bool, "can move around") ||
        !bot->CanFreeMove() ||
        !botAI->CanMove()) return false;

    return true;
}

bool MoveOutOfCollisionAction::Execute(Event event)
{
    float angle = static_cast<float>(M_PI) * 2000.0f / frand(1.0f, 1000.0f);
    float distance = sPlayerbotAIConfig->followDistance;
    return MoveTo(bot->GetMapId(),
        bot->GetPositionX() + cos(angle) * distance,
        bot->GetPositionY() + sin(angle) * distance,
        bot->GetPositionZ());
}

bool MoveOutOfCollisionAction::isUseful()
{
    // do not avoid collision on vehicle
    if (botAI->IsInVehicle())
        return false;

    GuidVector const& nearby = AI_VALUE(GuidVector, "nearest friendly players");
    if (nearby.size() >= 15)
        return false;

    return AI_VALUE2(bool, "collision", "self target");
}

bool AvoidAoeAction::isUseful()
{
    return sPlayerbotAIConfig->autoAvoidAoe && botAI->CanMove();
}

bool AvoidAoeAction::Execute(Event /*event*/)
{
    if (!sPlayerbotAIConfig->autoAvoidAoe || !botAI->CanMove())
        return false;

    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    float radius = std::max(sPlayerbotAIConfig->aoeRadius, 5.0f);
    float distanceToTarget = bot->GetDistance(target);
    if (distanceToTarget < radius)
        return MoveAway(target, radius - distanceToTarget);

    return false;
}

Unit* MoveAwayFromCreatureAction::GetCreature() const
{
    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && unit->GetEntry() == creatureEntry)
            return unit;
    }

    return nullptr;
}

bool MoveAwayFromCreatureAction::isUseful()
{
    Unit* creature = GetCreature();
    if (!creature)
        return false;

    return bot->GetDistance(creature) < distance;
}

bool MoveAwayFromCreatureAction::Execute(Event /*event*/)
{
    Unit* creature = GetCreature();
    if (!creature)
        return false;

    return MoveAway(creature, distance - bot->GetDistance(creature));
}

Player* MoveAwayFromPlayerWithDebuffAction::GetPlayerWithDebuff() const
{
    Group* group = bot->GetGroup();
    if (!group)
        return nullptr;

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || member == bot)
            continue;

        if (botAI->HasAura(auraId, member))
            return member;
    }

    return nullptr;
}

bool MoveAwayFromPlayerWithDebuffAction::isUseful()
{
    Player* player = GetPlayerWithDebuff();
    if (!player)
        return false;

    return bot->GetDistance(player) < distance;
}

bool MoveAwayFromPlayerWithDebuffAction::Execute(Event /*event*/)
{
    Player* player = GetPlayerWithDebuff();
    if (!player)
        return false;

    return MoveAway(player, distance - bot->GetDistance(player));
}

bool MovementAction::Follow(Unit* target, float distance) { return Follow(target, distance, GetFollowAngle()); }
bool MovementAction::Follow(Unit* target, float distance, float angle)
{
    MotionMaster& mm = *bot->GetMotionMaster();

    if (target)
        distance = distance <= target->GetCombatReach() ? 0.0f : distance - target->GetCombatReach();

    UpdateMovementState();

    if (FollowOnTransport(target))
        return true;

    if (!target)
        return false;

    float const desiredFollowDistance = std::max(0.0f, distance);

    // Move to target corpse if alive.
    if (!target->IsAlive() && bot->IsAlive() && target->GetGUID().IsPlayer())
    {
        Player* pTarget = (Player*)target;

        Corpse* corpse = pTarget->GetCorpse();

        if (corpse)
        {
            WorldPosition botPos(bot);
            WorldPosition cPos(corpse);

            if (botPos.fDist(cPos) > sPlayerbotAIConfig->spellDistance)
                return MoveTo(cPos.getMapId(), cPos.getX(), cPos.getY(), cPos.getZ());
        }
    }

    if (sServerFacade->IsDistanceGreaterOrEqualThan(sServerFacade->GetDistance2d(bot, target),
        sPlayerbotAIConfig->sightDistance) || target->HasUnitState(UNIT_STATE_IN_FLIGHT))
    {
        if (target->GetGUID().IsPlayer())
        {
            Player* pTarget = (Player*)target;

            PlayerbotAI* targetBotAI = GET_PLAYERBOT_AI(pTarget);
            if (targetBotAI)  // Try to move to where the bot is going if it is closer and in the same direction.
            {
                WorldPosition botPos(bot);
                WorldPosition tarPos(target);
                WorldPosition longMove =
                    targetBotAI->GetAiObjectContext()->GetValue<WorldPosition>("last long move")->Get();

                if (longMove)
                {
                    float lDist = botPos.fDist(longMove);
                    float tDist = botPos.fDist(tarPos);
                    float ang = botPos.getAngleBetween(tarPos, longMove);
                    if ((lDist * 1.5f < tDist && ang < static_cast<float>(M_PI) / 2) ||
                        target->HasUnitState(UNIT_STATE_IN_FLIGHT))
                    {
                        return MoveTo(longMove.getMapId(), longMove.getX(), longMove.getY(), longMove.getZ());
                    }
                }
            }
            else
            {
                if (pTarget->HasUnitState(UNIT_STATE_IN_FLIGHT))  // Move to where the player is flying to.
                {
                    TaxiPathNodeList const& tMap = static_cast<FlightPathMovementGenerator*>(pTarget->GetMotionMaster()->top())->GetPath();
                    if (!tMap.empty())
                    {
                        /*const TaxiPathNodeEntry& tEnd = tMap[tMap.GetTotalLength()];
                        return MoveTo(tEnd.MapId, tEnd.LocX, tEnd.LocY, tEnd.LocZ);*/
                    }
                }
            }
        }

        if (!target->HasUnitState(UNIT_STATE_IN_FLIGHT))
            return MoveTo(target, desiredFollowDistance);
    }

    bot->HandleEmoteCommand(0);

    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        botAI->InterruptSpell();
    }

    AI_VALUE(LastMovement&, "last movement").Set(target);
    ClearIdleState();

    if (bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == FOLLOW_MOTION_TYPE)
    {
        Unit* currentTarget = sServerFacade->GetChaseTarget(bot);
        if (currentTarget && currentTarget->GetGUID() == target->GetGUID())
            return false;
    }

    ClearBackwardMovementFlag(bot);
    mm.MoveFollow(target, desiredFollowDistance, angle);
    return true;
}

const Movement::PointsArray MovementAction::SearchForBestPath(float x, float y, float z, float& modified_z,
    int maxSearchCount, bool normal_only, float step)
{
    bool found = false;
    modified_z = INVALID_HEIGHT;
    float tempZ = bot->GetMapHeight(x, y, z);
    PathGenerator gen(bot);
    gen.CalculatePath(x, y, tempZ);
    Movement::PointsArray result = gen.GetPath();
    float min_length = gen.getPathLength();
    int typeOk = PATHFIND_NORMAL | PATHFIND_INCOMPLETE;
    if ((gen.GetPathType() & typeOk) && abs(tempZ - z) < 0.5f)
    {
        modified_z = tempZ;
        return result;
    }
    // Start searching
    if (gen.GetPathType() & typeOk)
    {
        modified_z = tempZ;
        found = true;
    }
    int count = 1;
    for (float delta = step; count < maxSearchCount / 2 + 1; count++, delta += step)
    {
        tempZ = bot->GetMapHeight(x, y, z + delta);
        if (tempZ == INVALID_HEIGHT)
        {
            continue;
        }
        PathGenerator gen(bot);
        gen.CalculatePath(x, y, tempZ);
        if ((gen.GetPathType() & typeOk) && gen.getPathLength() < min_length)
        {
            found = true;
            min_length = gen.getPathLength();
            result = gen.GetPath();
            modified_z = tempZ;
        }
    }
    for (float delta = -step; count < maxSearchCount; count++, delta -= step)
    {
        tempZ = bot->GetMapHeight(x, y, z + delta);
        if (tempZ == INVALID_HEIGHT)
        {
            continue;
        }
        PathGenerator gen(bot);
        gen.CalculatePath(x, y, tempZ);
        if ((gen.GetPathType() & typeOk) && gen.getPathLength() < min_length)
        {
            found = true;
            min_length = gen.getPathLength();
            result = gen.GetPath();
            modified_z = tempZ;
        }
    }
    if (!found && normal_only)
    {
        modified_z = INVALID_HEIGHT;
        return Movement::PointsArray{};
    }
    if (!found && !normal_only)
    {
        return result;
    }
    return result;
}

bool MovementAction::IsHazardNearPosition(WorldPosition position)
{
    if (!bot || !bot->GetMap() || !position || position.getMapId() != bot->GetMapId())
        return false;

    LiquidData liquidData;
    ZLiquidStatus liquidStatus = bot->GetMap()->GetLiquidStatus(
        position.getMapId(), position.getX(), position.getY(), position.getZ(), MAP_ALL_LIQUIDS, &liquidData,
        bot->GetCollisionHeight());

    return liquidStatus != LIQUID_MAP_NO_WATER &&
        (liquidData.type_flags & (MAP_LIQUID_TYPE_MAGMA | MAP_LIQUID_TYPE_SLIME));
}

bool MovementAction::IsValidPosition(WorldPosition position, WorldPosition visibleFromPosition)
{
    if (!position || !visibleFromPosition || !bot)
        return false;

    if (position.getMapId() != bot->GetMapId())
        return false;

    if (!bot->IsWithinLOS(position.getX(), position.getY(), position.getZ()))
        return false;

    WorldPosition botPosition(bot);
    if (!botPosition.canPathTo(position, bot))
        return false;

    return !IsHazardNearPosition(position);
}

bool MovementAction::GeneratePathAvoidingHazards(WorldPosition endPosition, bool generatePath, Movement::PointsArray& outPath)
{
    if (!generatePath || !bot || !bot->GetMap() || !endPosition || endPosition.getMapId() != bot->GetMapId())
        return false;

    PathGenerator basePath(bot);
    basePath.CalculatePath(endPosition.getX(), endPosition.getY(), endPosition.getZ(), false);
    Movement::PointsArray basePoints = basePath.GetPath();
    if (basePoints.empty())
        return false;

    bool pathHasHazard = false;
    for (G3D::Vector3 const& point : basePoints)
    {
        if (IsHazardNearPosition(WorldPosition(bot->GetMapId(), point.x, point.y, point.z, bot->GetOrientation())))
        {
            pathHasHazard = true;
            break;
        }
    }

    if (!pathHasHazard)
        return false;

    WorldPosition const botPosition(bot);
    float const baseRadius = std::max(4.0f, sPlayerbotAIConfig->contactDistance);
    float const maxRadius = std::max(12.0f, sPlayerbotAIConfig->aoeRadius * 2.0f);
    float bestLength = std::numeric_limits<float>::max();
    Movement::PointsArray bestPath;

    for (float radius = baseRadius; radius <= maxRadius; radius += 4.0f)
    {
        for (float angle = 0.0f; angle < static_cast<float>(2.0 * M_PI); angle += static_cast<float>(M_PI / 4.0f))
        {
            float x = endPosition.getX() + std::cos(angle) * radius;
            float y = endPosition.getY() + std::sin(angle) * radius;
            float z = endPosition.getZ();
            bot->UpdateAllowedPositionZ(x, y, z);

            WorldPosition candidate(bot->GetMapId(), x, y, z, bot->GetOrientation());
            if (!IsValidPosition(candidate, botPosition))
                continue;

            PathGenerator candidatePath(bot);
            candidatePath.CalculatePath(candidate.getX(), candidate.getY(), candidate.getZ(), false);
            int typeOk = PATHFIND_NORMAL | PATHFIND_INCOMPLETE;
            if (!(candidatePath.GetPathType() & typeOk))
                continue;

            Movement::PointsArray points = candidatePath.GetPath();
            if (points.empty())
                continue;

            bool candidateHasHazard = false;
            for (G3D::Vector3 const& point : points)
            {
                if (IsHazardNearPosition(WorldPosition(bot->GetMapId(), point.x, point.y, point.z, bot->GetOrientation())))
                {
                    candidateHasHazard = true;
                    break;
                }
            }

            if (candidateHasHazard)
                continue;

            float pathLength = candidatePath.getPathLength();
            if (pathLength < bestLength)
            {
                bestLength = pathLength;
                bestPath = std::move(points);
            }
        }
    }

    if (bestPath.empty())
        return false;

    outPath = std::move(bestPath);
    return true;
}

bool MovementAction::MoveAway(Unit* target, float distance, bool backwards)
{
    if (!target)
    {
        return false;
    }
    float init_angle = target->GetAngle(bot);
    for (float delta = 0; delta <= M_PI / 2; delta += M_PI / 8)
    {
        float angle = init_angle + delta;
        float dx = bot->GetPositionX() + cos(angle) * distance;
        float dy = bot->GetPositionY() + sin(angle) * distance;
        float dz = bot->GetPositionZ();
        bool exact = true;
        if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(),
            bot->GetPositionZ(), dx, dy, dz))
        {
            // disable prediction if position is invalid
            dx = bot->GetPositionX() + cos(angle) * distance;
            dy = bot->GetPositionY() + sin(angle) * distance;
            dz = bot->GetPositionZ();
            exact = false;
        }
        if (MoveTo(target->GetMapId(), dx, dy, dz, false, false, true, exact, MovementPriority::MOVEMENT_COMBAT, false, backwards))
        {
            return true;
        }
        if (delta == 0)
        {
            continue;
        }
        exact = true;
        angle = init_angle - delta;
        dx = bot->GetPositionX() + cos(angle) * distance;
        dy = bot->GetPositionY() + sin(angle) * distance;
        dz = bot->GetPositionZ();
        if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(),
            bot->GetPositionZ(), dx, dy, dz))
        {
            // disable prediction if position is invalid
            dx = bot->GetPositionX() + cos(angle) * distance;
            dy = bot->GetPositionY() + sin(angle) * distance;
            dz = bot->GetPositionZ();
            exact = false;
        }
        if (MoveTo(target->GetMapId(), dx, dy, dz, false, false, true, exact, MovementPriority::MOVEMENT_COMBAT, false, backwards))
        {
            return true;
        }
    }
    return false;
}

bool MovementAction::Move(float angle, float distance)
{
    float x = bot->GetPositionX() + cos(angle) * distance;
    float y = bot->GetPositionY() + sin(angle) * distance;

    //TODO do we need GetMapWaterOrGroundLevel() if we're using CheckCollisionAndGetValidCoords() ?
    float z = bot->GetMapWaterOrGroundLevel(x, y, bot->GetPositionZ());
    if (z == -100000.0f || z == -200000.0f)
        z = bot->GetPositionZ();
    if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(),
        bot->GetPositionZ(), x, y, z, false))
        return false;

    return MoveTo(bot->GetMapId(), x, y, z);
}

bool MovementAction::JumpTo(uint32 mapId, float x, float y, float z)
{
    if (!IsMovingAllowed())
        return false;

    if (bot->GetMapId() != mapId)
        return false;

    bot->GetMotionMaster()->MoveJump(x, y, z, bot->GetSpeed(MOVE_RUN), 7.0f);
    return true;
}

bool MovementAction::MoveInside(uint32 mapId, float x, float y, float z, float distance, MovementPriority priority)
{
    if (!IsMovingAllowed())
        return false;

    return MoveNear(mapId, x, y, z, distance, priority);
}

// just calculates average position of group and runs away from that position
bool MovementAction::MoveFromGroup(float distance)
{
    if (Group* group = bot->GetGroup())
    {
        uint32 mapId = bot->GetMapId();
        float closestDist = FLT_MAX;
        float x = 0.0f;
        float y = 0.0f;
        uint32 count = 0;

        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* player = gref->GetSource();
            if (!player || player == bot || !player->IsAlive() || player->GetMapId() != mapId)
                continue;
            float dist = bot->GetDistance2d(player);
            if (closestDist > dist)
                closestDist = dist;
            x += player->GetPositionX();
            y += player->GetPositionY();
            count++;
        }

        if (count && closestDist < distance)
        {
            x /= count;
            y /= count;
            // x and y are now average position of the group members
            float angle = bot->GetAngle(x, y) + M_PI;
            return Move(angle, distance - closestDist);
        }
    }
    return false;
}

bool MovementAction::Flee(Unit* target)
{
    Player* master = GetMaster();
    if (!target)
        target = master;

    if (!target)
        return false;

    if (!sPlayerbotAIConfig->fleeingEnabled)
        return false;

    if (!IsMovingAllowed())
    {
        botAI->TellError("I am stuck while fleeing");
        return false;
    }

    LastMovement& lastMovement = AI_VALUE(LastMovement&, "last movement");
    time_t now = time(nullptr);
    uint32 maxDelay = std::max<uint32>(2, sPlayerbotAIConfig->returnDelay / 1000);
    uint32 fleeDelay = urand(2, maxDelay);

    if (lastMovement.lastFlee && bot->isMoving() && (now - lastMovement.lastFlee) <= fleeDelay)
        return true;

    bool const isTarget = target->GetVictim() == bot;
    bool const isHealer = PlayerBotSpec::IsHeal(bot);
    bool const isRanged = PlayerBotSpec::IsRanged(bot);
    bool const needHealer = !isHealer && AI_VALUE2(uint8, "health", "self target") < 50;

    Unit* fleeTarget = nullptr;
    std::vector<Unit*> possibleTargets;
    if (Group* group = bot->GetGroup())
    {
        float const minFleeDistance = 5.0f;
        float const maxFleeDistance = isTarget ? 40.0f : botAI->GetRange("spell") * 1.5f;
        float const minRangedTargetDistance = botAI->GetRange("spell") / 2.0f + botAI->GetRange("follow");

        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            if (!member || member == bot || member == master || !member->IsAlive() || member->GetMapId() != bot->GetMapId())
                continue;

            float const distanceToMember = sServerFacade->GetDistance2d(bot, member);
            if (distanceToMember < minFleeDistance || distanceToMember > maxFleeDistance)
                continue;

            if (isTarget)
            {
                if (PlayerBotSpec::IsTank(member))
                    possibleTargets.push_back(member);
                continue;
            }

            float const distanceToTarget = sServerFacade->GetDistance2d(member, target);
            if (((isHealer && PlayerBotSpec::IsHeal(member)) || needHealer) &&
                distanceToTarget > minRangedTargetDistance &&
                (needHealer || member->IsWithinLOSInMap(target)))
            {
                possibleTargets.push_back(member);
            }
            else if (isRanged && PlayerBotSpec::IsRanged(member) &&
                distanceToTarget > minRangedTargetDistance &&
                member->IsWithinLOSInMap(target))
            {
                possibleTargets.push_back(member);
            }
        }
    }

    if (!possibleTargets.empty())
    {
        fleeTarget = possibleTargets[urand(0, possibleTargets.size() - 1)];
    }
    else if (master && master->IsAlive() && master->GetMapId() == bot->GetMapId() && master->IsWithinLOSInMap(target))
    {
        float const distanceToMaster = sServerFacade->GetDistance2d(bot, master);
        float const minFleeDistance = 5.0f;
        float const maxFleeDistance = isTarget ? 40.0f : botAI->GetRange("spell") * 1.5f;
        if (distanceToMaster > minFleeDistance && distanceToMaster < maxFleeDistance)
        {
            if (!isRanged ||
                sServerFacade->GetDistance2d(master, target) > (botAI->GetRange("spell") / 2.0f + botAI->GetRange("follow")))
            {
                fleeTarget = master;
            }
        }
    }

    if (fleeTarget && MoveNear(fleeTarget, sPlayerbotAIConfig->contactDistance, MovementPriority::MOVEMENT_COMBAT))
    {
        lastMovement.lastFlee = now;
        return true;
    }

    float fleeDistance = botAI->GetRange("flee");
    if (target->IsPlayer())
        fleeDistance *= 2.0f;

    bool moved = MoveAway(target, fleeDistance);
    if (moved)
        lastMovement.lastFlee = now;

    return moved;
}

Position MovementAction::BestPositionForMeleeToFlee(Position pos, float radius)
{
    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    std::vector<CheckAngle> possibleAngles;
    if (currentTarget)
    {
        // Normally, move to left or right is the best position
        bool isTanking = (currentTarget->CanFreeMove()) && (currentTarget->GetVictim() == bot);
        float angle = bot->GetAngle(currentTarget);
        float angleLeft = angle + (float)M_PI / 2;
        float angleRight = angle - (float)M_PI / 2;
        possibleAngles.push_back({ angleLeft, false });
        possibleAngles.push_back({ angleRight, false });
        possibleAngles.push_back({ angle, true });
        if (isTanking)
        {
            possibleAngles.push_back({ angle + (float)M_PI, false });
            possibleAngles.push_back({ bot->GetAngle(&pos) - (float)M_PI, false });
        }
    }
    else
    {
        float angleTo = bot->GetAngle(&pos) - (float)M_PI;
        possibleAngles.push_back({ angleTo, false });
    }
    float farestDis = 0.0f;
    Position bestPos;
    for (CheckAngle& checkAngle : possibleAngles)
    {
        float angle = checkAngle.angle;
        std::list<FleeInfo>& infoList = AI_VALUE(std::list<FleeInfo>&, "recently flee info");
        if (!CheckLastFlee(angle, infoList))
        {
            continue;
        }
        bool strict = checkAngle.strict;
        float fleeDis = std::min(radius + 1.0f, sPlayerbotAIConfig->fleeDistance);
        float dx = bot->GetPositionX() + cos(angle) * fleeDis;
        float dy = bot->GetPositionY() + sin(angle) * fleeDis;
        float dz = bot->GetPositionZ();
        if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(),
            bot->GetPositionZ(), dx, dy, dz))
        {
            continue;
        }
        Position fleePos{ dx, dy, dz };
        if (strict && currentTarget &&
            fleePos.GetExactDist(currentTarget) - currentTarget->GetCombatReach() >
            sPlayerbotAIConfig->tooCloseDistance &&
            bot->IsWithinMeleeRange(currentTarget))
        {
            continue;
        }
        if (pos.GetExactDist(fleePos) > farestDis)
        {
            farestDis = pos.GetExactDist(fleePos);
            bestPos = fleePos;
        }
    }
    if (farestDis > 0.0f)
    {
        return bestPos;
    }
    return Position();
}

Position MovementAction::BestPositionForRangedToFlee(Position pos, float radius)
{
    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    std::vector<CheckAngle> possibleAngles;
    float angleToTarget = 0.0f;
    float angleFleeFromCenter = bot->GetAngle(&pos) - (float)M_PI;
    if (currentTarget)
    {
        // Normally, move to left or right is the best position
        angleToTarget = bot->GetAngle(currentTarget);
        float angleLeft = angleToTarget + (float)M_PI / 2;
        float angleRight = angleToTarget - (float)M_PI / 2;
        possibleAngles.push_back({ angleLeft, false });
        possibleAngles.push_back({ angleRight, false });
        possibleAngles.push_back({ angleToTarget + (float)M_PI, true });
        possibleAngles.push_back({ angleToTarget, true });
        possibleAngles.push_back({ angleFleeFromCenter, true });
    }
    else
    {
        possibleAngles.push_back({ angleFleeFromCenter, false });
    }
    float farestDis = 0.0f;
    Position bestPos;
    for (CheckAngle& checkAngle : possibleAngles)
    {
        float angle = checkAngle.angle;
        std::list<FleeInfo>& infoList = AI_VALUE(std::list<FleeInfo>&, "recently flee info");
        if (!CheckLastFlee(angle, infoList))
        {
            continue;
        }
        bool strict = checkAngle.strict;
        float fleeDis = std::min(radius + 1.0f, sPlayerbotAIConfig->fleeDistance);
        float dx = bot->GetPositionX() + cos(angle) * fleeDis;
        float dy = bot->GetPositionY() + sin(angle) * fleeDis;
        float dz = bot->GetPositionZ();
        if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(),
            bot->GetPositionZ(), dx, dy, dz))
        {
            continue;
        }
        Position fleePos{ dx, dy, dz };
        if (strict && currentTarget &&
            fleePos.GetExactDist(currentTarget) - currentTarget->GetCombatReach() > sPlayerbotAIConfig->spellDistance)
        {
            continue;
        }
        if (strict && currentTarget &&
            fleePos.GetExactDist(currentTarget) - currentTarget->GetCombatReach() <
            (sPlayerbotAIConfig->tooCloseDistance))
        {
            continue;
        }

        if (pos.GetExactDist(fleePos) > farestDis)
        {
            farestDis = pos.GetExactDist(fleePos);
            bestPos = fleePos;
        }
    }
    if (farestDis > 0.0f)
    {
        return bestPos;
    }
    return Position();
}

bool MovementAction::FleePosition(Position pos, float radius, uint32 minInterval)
{
    std::list<FleeInfo>& infoList = AI_VALUE(std::list<FleeInfo>&, "recently flee info");

    if (!infoList.empty() && infoList.back().timestamp + minInterval > getMSTime())
        return false;

    Position bestPos;
    if (PlayerBotSpec::IsMelee(bot))
    {
        bestPos = BestPositionForMeleeToFlee(pos, radius);
    }
    else
    {
        bestPos = BestPositionForRangedToFlee(pos, radius);
    }
    if (bestPos != Position())
    {
        if (MoveTo(bot->GetMapId(), bestPos.GetPositionX(), bestPos.GetPositionY(), bestPos.GetPositionZ(), false,
            false, true, false, MovementPriority::MOVEMENT_COMBAT))
        {
            uint32 curTS = getMSTime();
            while (!infoList.empty())
            {
                if (infoList.size() > 10 || infoList.front().timestamp + 5000 < curTS)
                {
                    infoList.pop_front();
                }
                else
                {
                    break;
                }
            }
            infoList.push_back({ pos, radius, bot->GetAngle(&bestPos), curTS });
            return true;
        }
    }
    return false;
}

bool MovementAction::CheckLastFlee(float curAngle, std::list<FleeInfo>& infoList)
{
    uint32 curTS = getMSTime();
    curAngle = Position::NormalizeOrientation(curAngle);
    while (!infoList.empty())
    {
        if (infoList.size() > 10 || infoList.front().timestamp + 5000 < curTS)
        {
            infoList.pop_front();
        }
        else
        {
            break;
        }
    }
    for (FleeInfo& info : infoList)
    {
        // more than 5 sec
        if (info.timestamp + 5000 < curTS)
        {
            continue;
        }
        float revAngle = Position::NormalizeOrientation(info.angle + M_PI);
        // angle too close
        if (fabs(revAngle - curAngle) < M_PI / 4)
        {
            return false;
        }
    }
    return true;
}

bool RunAwayAction::Execute(Event event) { return Flee(AI_VALUE(Unit*, "master target")); }

bool FleeWithPetAction::Execute(Event event)
{
    if (Unit* pet = bot->GetPet())
    {
        if (Creature* petCreature = pet->ToCreature())
            petCreature->SetReactState(REACT_PASSIVE);
        pet->AttackStop();
    }

    return Flee(AI_VALUE(Unit*, "current target"));
}

bool MoveToLootAction::Execute(Event /*event*/)
{
    LootObject loot = AI_VALUE(LootObject, "loot target");
    if (!loot.IsLootPossible(bot))
        return false;

    return MoveNear(loot.GetWorldObject(bot), sPlayerbotAIConfig->contactDistance);
}

bool MoveOutOfEnemyContactAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    return MoveTo(target, sPlayerbotAIConfig->contactDistance);
}

bool MoveOutOfEnemyContactAction::isUseful()
{
    return MovementAction::isUseful() && AI_VALUE2(bool, "inside target", "current target");
}

bool JumpAction::isUseful()
{
    if (!bot->IsInWorld())
        return false;

    if (bot->HasUnitState(UNIT_STATE_IN_FLIGHT) || bot->IsMounted())
        return false;

    return botAI->HasPlayerNearby();
}

bool JumpAction::Execute(Event event)
{
    if (!IsMovingAllowed())
        return false;

    // Keep jump commands player-driven while a cast is in progress.
    if (!event.getOwner() && bot->IsNonMeleeSpellCasted(false, false, true))
        return false;

    if (bot->IsMounted() || bot->HasUnitState(UNIT_STATE_IN_FLIGHT))
        return false;

    std::string const options = event.getParam();
    bool jumpInPlace = false;
    bool jumpBackward = false;

    if (options.empty() || options == "i" || options == "inplace")
    {
        jumpInPlace = true;
    }
    else if (options == "r" || options == "random")
    {
        jumpInPlace = urand(0, 99) < 20;
        jumpBackward = !jumpInPlace && urand(0, 99) < 20;
    }
    else if (options == "b" || options == "back" || options == "backward")
    {
        jumpBackward = true;
    }

    float angle = bot->GetOrientation();
    if (jumpBackward)
        angle = Position::NormalizeOrientation(angle + static_cast<float>(M_PI));

    float const verticalSpeed = 7.5f;
    float const jumpSpeed = jumpInPlace ? 0.0f : (jumpBackward ? bot->GetSpeed(MOVE_WALK) : bot->GetSpeedRate(MOVE_RUN) * 1.2f);

    float timeToLand = 0.0f;
    float distanceToLand = 0.0f;
    float maxHeight = 0.0f;
    bool goodLanding = true;
    std::vector<WorldPosition> path;
    WorldPosition jumpLanding = CalculateJumpParameters(
        WorldPosition(bot), bot, angle, verticalSpeed, jumpSpeed, timeToLand, distanceToLand, maxHeight, goodLanding, path);

    if (!jumpLanding)
        return false;

    WorldPosition highestPoint = jumpLanding;
    for (WorldPosition point : path)
        if (point && point.getZ() > highestPoint.getZ())
            highestPoint = point;

    return DoJump(
        jumpLanding,
        highestPoint,
        angle,
        verticalSpeed,
        jumpSpeed,
        timeToLand,
        distanceToLand,
        maxHeight,
        goodLanding,
        jumpInPlace,
        jumpBackward,
        false);
}

WorldPosition JumpAction::CalculateJumpParameters(
    WorldPosition src,
    Unit* jumper,
    float angle,
    float vSpeed,
    float hSpeed,
    float& timeToLand,
    float& distanceToLand,
    float& maxHeight,
    bool& goodLanding,
    std::vector<WorldPosition>& path,
    float maxJumpHeight)
{
    timeToLand = 0.0f;
    distanceToLand = 0.0f;
    maxHeight = 0.0f;
    goodLanding = false;
    path.clear();

    if (!jumper)
        return WorldPosition();

    float const gravity = 19.2911f;
    float const maxHeightTime = vSpeed / gravity;
    maxHeight = vSpeed * maxHeightTime - gravity * maxHeightTime * maxHeightTime * 0.5f;

    if (hSpeed <= 0.0f)
    {
        timeToLand = std::max(0.1f, maxHeightTime * 2.0f);
        distanceToLand = 0.0f;
        WorldPosition landing = src;
        if (!CanLand(landing, jumper) || !IsNotMagmaSlime(landing, jumper))
            return WorldPosition();

        goodLanding = true;
        path.push_back(landing);
        return landing;
    }

    float const distance = std::max(0.8f, hSpeed * 0.35f);
    float x = src.getX() + std::cos(angle) * distance;
    float y = src.getY() + std::sin(angle) * distance;
    float z = src.getZ();

    if (!jumper->GetMap()->CheckCollisionAndGetValidCoords(jumper, jumper->GetPositionX(), jumper->GetPositionY(),
        jumper->GetPositionZ(), x, y, z))
    {
        return WorldPosition();
    }

    WorldPosition landing(src.getMapId(), x, y, z, jumper->GetOrientation());
    if (!CanLand(landing, jumper) || !IsNotMagmaSlime(landing, jumper))
        return WorldPosition();

    float zDiff = landing.getZ() - src.getZ();
    timeToLand = CalculateJumpTime(zDiff, vSpeed, zDiff >= 0.0f);
    if (timeToLand <= 0.0f)
        return WorldPosition();

    distanceToLand = src.fDist(landing);
    maxHeight = src.getZ() + maxHeight;

    if (maxJumpHeight > 0.0f && std::fabs(landing.getZ() - src.getZ()) > maxJumpHeight)
        return WorldPosition();

    goodLanding = IsJumpSafe(src, landing, jumper);
    if (!goodLanding)
        return WorldPosition();

    path.push_back(landing);
    return landing;
}

bool JumpAction::DoJump(
    WorldPosition dest,
    WorldPosition /*highestPoint*/,
    float /*angle*/,
    float /*vSpeed*/,
    float /*hSpeed*/,
    float /*timeToLand*/,
    float /*distanceToLand*/,
    float /*maxHeight*/,
    bool /*goodLanding*/,
    bool /*jumpInPlace*/,
    bool /*jumpBackward*/,
    bool showOnly)
{
    if (showOnly || !dest)
        return false;

    return JumpTo(dest.getMapId(), dest.getX(), dest.getY(), dest.getZ());
}

bool JumpAction::JumpTowards(WorldPosition src, WorldPosition dest, Unit* jumper, float jumpSpeed, bool preSetLanding)
{
    if (!src || !dest || !jumper)
        return false;

    float angle = src.getAngleTo(dest);
    float timeToLand = 0.0f;
    float distanceToLand = 0.0f;
    float maxHeight = 0.0f;
    bool goodLanding = true;
    std::vector<WorldPosition> path;
    WorldPosition jumpLanding = CalculateJumpParameters(
        src, jumper, angle, 7.5f, jumpSpeed, timeToLand, distanceToLand, maxHeight, goodLanding, path);

    if (!jumpLanding)
        return false;

    WorldPosition finalDest = preSetLanding ? dest : jumpLanding;
    WorldPosition highestPoint = finalDest;
    for (WorldPosition point : path)
        if (point && point.getZ() > highestPoint.getZ())
            highestPoint = point;

    return DoJump(finalDest, highestPoint, angle, 7.5f, jumpSpeed, timeToLand, distanceToLand, maxHeight, goodLanding,
        false, false, false);
}

float JumpAction::CalculateJumpTime(float srcZ, float destZ, float vSpeed, float /*hSpeed*/, float /*distance*/)
{
    float zDiff = destZ - srcZ;
    return CalculateJumpTime(zDiff, vSpeed, zDiff >= 0.0f);
}

float JumpAction::CalculateJumpTime(float zDiff, float vSpeed, bool ascending)
{
    float const gravity = 19.2911f;
    float const discriminant = vSpeed * vSpeed - gravity * 2.0f * zDiff;
    if (discriminant < 0.0f)
        return 0.0f;

    float const root = std::sqrt(discriminant);
    return ascending ? (vSpeed - root) / gravity : (vSpeed + root) / gravity;
}

WorldPosition JumpAction::GetPossibleJumpStartFor(
    WorldPosition src,
    WorldPosition dest,
    WorldPosition& possibleLanding,
    Unit* jumper,
    float& requiredSpeed,
    float /*distanceTo*/,
    float distanceFrom)
{
    float jumpAngle = 0.0f;
    requiredSpeed = jumper ? jumper->GetSpeed(MOVE_WALK) : 0.0f;

    if (CanJumpTo(src, dest, possibleLanding, jumpAngle, jumper, requiredSpeed, distanceFrom))
        return src;

    if (jumper)
    {
        requiredSpeed = jumper->GetSpeedRate(MOVE_RUN) * 1.2f;
        if (CanJumpTo(src, dest, possibleLanding, jumpAngle, jumper, requiredSpeed, distanceFrom))
            return src;
    }

    return WorldPosition();
}

WorldPosition JumpAction::GetPossibleJumpStartForInRange(
    WorldPosition src,
    WorldPosition dest,
    WorldPosition& possibleLanding,
    Unit* jumper,
    float& requiredSpeed,
    float distanceTo,
    float distanceFrom)
{
    WorldPosition start = GetPossibleJumpStartFor(src, dest, possibleLanding, jumper, requiredSpeed, distanceTo, distanceFrom);
    if (start)
        return start;

    for (float radius = 2.0f; radius <= std::max(4.0f, distanceTo); radius += 2.0f)
    {
        for (float angle = 0.0f; angle < static_cast<float>(2.0 * M_PI); angle += static_cast<float>(M_PI / 4.0f))
        {
            float x = src.getX() + std::cos(angle) * radius;
            float y = src.getY() + std::sin(angle) * radius;
            float z = src.getZ();
            if (jumper && !jumper->GetMap()->CheckCollisionAndGetValidCoords(jumper, src.getX(), src.getY(), src.getZ(), x, y, z))
                continue;

            WorldPosition candidate(src.getMapId(), x, y, z, src.getO());
            start = GetPossibleJumpStartFor(candidate, dest, possibleLanding, jumper, requiredSpeed, distanceTo, distanceFrom);
            if (start)
                return start;
        }
    }

    return WorldPosition();
}

bool JumpAction::CanJumpTo(
    WorldPosition src,
    WorldPosition dest,
    WorldPosition& possibleLanding,
    float& jumpAngle,
    Unit* jumper,
    float jumpSpeed,
    float maxDistance)
{
    if (!src || !dest || !jumper)
        return false;

    if (src.getMapId() != dest.getMapId())
        return false;

    for (float angle = src.getAngleTo(dest); angle < src.getAngleTo(dest) + static_cast<float>(2.0 * M_PI);
        angle += static_cast<float>(M_PI / 4.0f))
    {
        float timeToLand = 0.0f;
        float distanceToLand = 0.0f;
        float maxHeight = 0.0f;
        bool goodLanding = true;
        std::vector<WorldPosition> path;
        WorldPosition landing = CalculateJumpParameters(src, jumper, angle, 7.5f, jumpSpeed, timeToLand, distanceToLand,
            maxHeight, goodLanding, path);

        if (!landing)
            continue;

        if (landing.fDist(dest) > maxDistance)
            continue;

        if (!CanWalkTo(landing, dest, jumper, maxDistance))
            continue;

        possibleLanding = landing;
        jumpAngle = angle;
        return true;
    }

    return false;
}

bool JumpAction::CanWalkTo(WorldPosition src, WorldPosition dest, Unit* jumper, float maxDistance)
{
    if (!src || !dest || !jumper)
        return false;

    if (src.getMapId() != dest.getMapId())
        return false;

    std::vector<WorldPosition> path = dest.getPathStepFrom(src, jumper);
    if (path.empty())
        return false;

    return src.getPathLength(path) <= maxDistance;
}

bool JumpAction::IsJumpFasterThanWalking(
    WorldPosition src,
    WorldPosition dest,
    WorldPosition jumpLanding,
    Unit* jumper,
    float maxDistance)
{
    if (!src || !dest || !jumpLanding || !jumper)
        return false;

    if (jumpLanding.fDist(dest) > maxDistance)
        return false;

    std::vector<WorldPosition> walkPath = dest.getPathStepFrom(src, jumper);
    std::vector<WorldPosition> jumpPath = dest.getPathStepFrom(jumpLanding, jumper);
    if (jumpPath.empty())
        return false;
    if (walkPath.empty())
        return true;

    return src.getPathLength(walkPath) > (jumpLanding.getPathLength(jumpPath) * 1.5f);
}

bool JumpAction::IsJumpSafe(WorldPosition src, WorldPosition dest, Unit* jumper)
{
    if (!src || !dest || !jumper)
        return false;

    return CanLand(dest, jumper) && IsNotMagmaSlime(dest, jumper);
}

bool JumpAction::CanLand(WorldPosition dest, Unit* jumper)
{
    if (!dest || !jumper || !jumper->GetMap())
        return false;

    float groundZ = jumper->GetMap()->GetHeight(dest.getX(), dest.getY(), dest.getZ());
    return groundZ != INVALID_HEIGHT;
}

bool JumpAction::IsNotMagmaSlime(WorldPosition dest, Unit* jumper)
{
    if (!dest || !jumper || !jumper->GetMap())
        return false;

    LiquidData liquidData;
    ZLiquidStatus liquidStatus = jumper->GetMap()->GetLiquidStatus(
        dest.getMapId(), dest.getX(), dest.getY(), dest.getZ(), MAP_ALL_LIQUIDS, &liquidData,
        jumper->GetCollisionHeight());
    if (liquidStatus == LIQUID_MAP_NO_WATER)
        return true;

    return !(liquidData.type_flags & (MAP_LIQUID_TYPE_MAGMA | MAP_LIQUID_TYPE_SLIME));
}

bool SetFacingTargetAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    if (bot->HasUnitState(UNIT_STATE_IN_FLIGHT))
        return true;

    sServerFacade->SetFacingTo(bot, target);
    botAI->SetNextCheckDelay(sPlayerbotAIConfig->reactDelay);
    return true;
}

bool SetFacingTargetAction::isUseful()
{
    return !AI_VALUE2(bool, "facing", "current target");
}

bool SetFacingTargetAction::isPossible()
{
    if (bot->isFrozen() || bot->IsPolymorphed() || (bot->isDead() && !bot->HasPlayerFlag(PLAYER_FLAGS_GHOST)) ||
        bot->IsBeingTeleported() || bot->HasConfuseAura() || bot->IsCharmed() ||
        bot->HasStunAura() || bot->IsInFlight() ||
        bot->HasUnitState(UNIT_STATE_LOST_CONTROL))
        return false;

    return true;
}

bool SetBehindTargetAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    float distance = std::max(0.5f, (target->GetCombatReach() + bot->GetCombatReach()) * 0.8f);
    float x = target->GetPositionX() - std::cos(target->GetOrientation()) * distance;
    float y = target->GetPositionY() - std::sin(target->GetOrientation()) * distance;
    float z = target->GetPositionZ();
    if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(),
        x, y, z))
    {
        float angle = GetFollowAngle() / 3.0f + target->GetOrientation() + M_PI_2;
        distance = std::max(0.5f, sPlayerbotAIConfig->contactDistance);
        x = target->GetPositionX() + std::cos(angle) * distance;
        y = target->GetPositionY() + std::sin(angle) * distance;
        z = target->GetPositionZ();
        if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(),
            x, y, z))
            return false;
    }

    return MoveTo(bot->GetMapId(), x, y, z, false, false, false, true, MovementPriority::MOVEMENT_COMBAT);
}

bool SetBehindTargetAction::isUseful()
{
    if (!MovementAction::isUseful())
        return false;

    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    if (AI_VALUE2(bool, "behind", "current target"))
        return false;

    return bot->GetDistance(target) <= 15.0f;
}

bool SetBehindTargetAction::isPossible()
{
    if (!IsMovingAllowed())
        return false;

    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    if (Player* playerTarget = target->ToPlayer())
        return playerTarget->GetTarget() != bot->GetGUID();

    return !(target->GetVictim() && target->GetVictim() == bot);
}

bool MoveFromGroupAction::Execute(Event event)
{
    float distance = atoi(event.getParam().c_str());
    if (!distance)
        distance = 20.0f; // flee distance from config is too small for this
    return MoveFromGroup(distance);
}

bool FleeAction::Execute(Event event)
{
    return Flee(AI_VALUE(Unit*, "current target"));
}

bool FleeAction::isUseful()
{
    if (bot->GetCurrentSpell(CURRENT_CHANNELED_SPELL) != nullptr)
    {
        return false;
    }
    return true;
}

bool CombatFormationMoveAction::isUseful()
{
    if (getMSTime() - moveInterval < lastMoveTimer)
    {
        return false;
    }
    if (bot->GetCurrentSpell(CURRENT_CHANNELED_SPELL) != nullptr)
    {
        return false;
    }
    return true;
}

bool CombatFormationMoveAction::Execute(Event event)
{
    float dis = AI_VALUE(float, "disperse distance");
    if (dis <= 0.0f)
        return false;
    Player* playerToLeave = NearestGroupMember(dis);
    if (playerToLeave && bot->GetExactDist(playerToLeave) < dis)
    {
        //if (FleePosition(playerToLeave->GetPosition(), dis))
        {
            //lastMoveTimer = getMSTime();
        }
    }
    return false;
}

Position CombatFormationMoveAction::AverageGroupPos(float dis, bool ranged, bool self)
{
    float averageX = 0, averageY = 0, averageZ = 0;
    int cnt = 0;
    Group* group = bot->GetGroup();
    if (!group)
    {
        return Position();
    }
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        Player* member = ObjectAccessor::FindPlayer(itr->guid);
        if (!member)
            continue;

        if (!self && member == bot)
            continue;

        if (ranged && !PlayerBotSpec::IsRanged(member))
            continue;

        if (!member->IsAlive() || member->GetMapId() != bot->GetMapId() || member->IsCharmed() ||
            sServerFacade->GetDistance2d(bot, member) > dis)
            continue;

        averageX += member->GetPositionX();
        averageY += member->GetPositionY();
        averageZ += member->GetPositionZ();
    }
    averageX /= cnt;
    averageY /= cnt;
    averageZ /= cnt;
    return Position(averageX, averageY, averageZ);
}

float CombatFormationMoveAction::AverageGroupAngle(Unit* from, bool ranged, bool self)
{
    Group* group = bot->GetGroup();
    if (!from || !group)
    {
        return 0.0f;
    }
    // float average = 0.0f;
    float sumX = 0.0f;
    float sumY = 0.0f;
    int cnt = 0;
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        Player* member = ObjectAccessor::FindPlayer(itr->guid);
        if (!member)
            continue;

        if (!self && member == bot)
            continue;

        if (ranged && !PlayerBotSpec::IsRanged(member))
            continue;

        if (!member->IsAlive() || member->GetMapId() != bot->GetMapId() || member->IsCharmed() ||
            sServerFacade->GetDistance2d(bot, member) > sPlayerbotAIConfig->sightDistance)
            continue;

        cnt++;
        sumX += member->GetPositionX() - from->GetPositionX();
        sumY += member->GetPositionY() - from->GetPositionY();
    }
    if (cnt == 0)
        return 0.0f;

    // unnecessary division
    // sumX /= cnt;
    // sumY /= cnt;

    return atan2(sumY, sumX);
}

Position CombatFormationMoveAction::GetNearestPosition(const std::vector<Position>& positions)
{
    Position result;
    for (const Position& pos : positions)
    {
        if (bot->GetExactDist(pos) < bot->GetExactDist(result))
            result = pos;
    }
    return result;
}

Player* CombatFormationMoveAction::NearestGroupMember(float dis)
{
    float nearestDis = 10000.0f;
    Player* result = nullptr;
    Group* group = bot->GetGroup();
    if (!group)
    {
        return result;
    }
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        Player* member = ObjectAccessor::FindPlayer(itr->guid);
        if (!member || !member->IsAlive() || member == bot || member->GetMapId() != bot->GetMapId() ||
            member->IsCharmed() || sServerFacade->GetDistance2d(bot, member) > dis)
            continue;
        if (nearestDis > bot->GetExactDist(member))
        {
            result = member;
            nearestDis = bot->GetExactDist(member);
        }
    }
    return result;
}

bool TankFaceAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    if (!bot->GetGroup())
        return false;

    if (!bot->IsWithinMeleeRange(target) || target->isMoving())
        return false;

    if (!AI_VALUE2(bool, "has aggro", "current target"))
        return false;

    float averageAngle = AverageGroupAngle(target, true);

    if (averageAngle == 0.0f)
        return false;

    float deltaAngle = Position::NormalizeOrientation(averageAngle - target->GetAngle(bot));
    if (deltaAngle > M_PI)
        deltaAngle -= 2.0f * M_PI; // -PI..PI

    float tolerable = M_PI_2;

    if (fabs(deltaAngle) > tolerable)
        return false;

    float goodAngle1 = Position::NormalizeOrientation(averageAngle + M_PI * 3 / 5);
    float goodAngle2 = Position::NormalizeOrientation(averageAngle - M_PI * 3 / 5);

    // if dist < bot->GetMeleeRange(target) / 2, target will move backward
    float dist = std::max(bot->GetExactDist(target), bot->GetMeleeRange(target) / 2) - bot->GetCombatReach() - target->GetCombatReach();
    std::vector<Position> availablePos;
    float x, y, z;
    target->GetNearPoint(bot, x, y, z, 0.0f, dist, goodAngle1);
    if (bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(),
        x, y, z))
    {
        /// @todo: movement control now is a mess, prepare to rewrite
        std::list<FleeInfo>& infoList = AI_VALUE(std::list<FleeInfo>&, "recently flee info");
        Position pos(x, y, z);
        float angle = bot->GetAngle(&pos);
        if (CheckLastFlee(angle, infoList))
        {
            availablePos.push_back(Position(x, y, z));
        }
    }
    target->GetNearPoint(bot, x, y, z, 0.0f, dist, goodAngle2);
    if (bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(),
        x, y, z))
    {
        std::list<FleeInfo>& infoList = AI_VALUE(std::list<FleeInfo>&, "recently flee info");
        Position pos(x, y, z);
        float angle = bot->GetAngle(&pos);
        if (CheckLastFlee(angle, infoList))
        {
            availablePos.push_back(Position(x, y, z));
        }
    }
    if (availablePos.empty())
        return false;
    Position nearest = GetNearestPosition(availablePos);
    return MoveTo(bot->GetMapId(), nearest.GetPositionX(), nearest.GetPositionY(), nearest.GetPositionZ(), false, false, false, true, MovementPriority::MOVEMENT_COMBAT);
}
