#include "NewRpgAction.h"

#include <cmath>
#include <cstdint>
#include <list>
#include <vector>

#include "NewRpgStrategy.h"
#include "Helper.h"
#include "ObjectDefines.h"
#include "ObjectGuid.h"
#include "PathGenerator.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "Random.h"
#include "RandomPlayerbotMgr.h"
#include "Timer.h"
#include "WorldPosition.h"
#include "World.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "SharedDefines.h"

namespace
{
    GuidPosition FindNearbyNpc(Player* bot, float range)
    {
        if (!bot)
            return GuidPosition();

        std::list<Unit*> targets;
        Trinity::AnyFriendlyUnitInObjectRangeCheck u_check(bot, bot, range);
        Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(bot, targets, u_check);
        Cell::VisitAllObjects(bot, searcher, range);

        std::vector<Unit*> candidates;
        candidates.reserve(targets.size());
        for (Unit* unit : targets)
        {
            if (!unit || unit->GetTypeId() != TYPEID_UNIT)
                continue;
            if (!unit->IsAlive() || unit->IsInCombat())
                continue;
            candidates.push_back(unit);
        }

        if (candidates.empty())
            return GuidPosition();

        Unit* chosen = candidates[urand(0, candidates.size() - 1)];
        return GuidPosition(chosen);
    }
}

bool TellRpgStatusAction::Execute(Event event)
{
    Player* owner = event.getOwner();
    if (!owner)
        owner = botAI->GetMaster();

    if (!owner)
        return false;

    std::string out = botAI->GetRpgInfo().ToString();
    botAI->Whisper(out, owner->GetName());
    return true;
}

bool NewRpgStatusUpdateAction::Execute(Event event)
{
    if (!bot || bot->IsInCombat())
        return false;

    NewRpgInfo& info = botAI->GetRpgInfo();
    uint32 now = getMSTime();

    auto trySetNearNpc = [&]() -> bool
    {
        GuidPosition npcPos = FindNearbyNpc(bot, sPlayerbotAIConfig->reactDistance);
        if (!npcPos)
            return false;

        info.npcPos = npcPos;
        info.lastNearNpc = now;
        info.lastReachNpc = 0;
        info.status = NewRpgStatus::NEAR_NPC;
        return true;
    };

    auto setNearRandom = [&]() -> bool
    {
        info.lastNearRandom = now;
        info.status = NewRpgStatus::NEAR_RANDOM;
        return true;
    };

    auto setRest = [&]() -> bool
    {
        info.status = NewRpgStatus::REST;
        info.lastRest = now;
        bot->SetStandState(UNIT_STAND_STATE_SIT);
        return true;
    };

    auto setGoGrind = [&]() -> bool
    {
        WorldPosition pos = SelectRandomGrindPos();
        if (!pos)
            return false;
        info.grindPos = pos;
        info.lastGoGrind = now;
        info.status = NewRpgStatus::GO_GRIND;
        return true;
    };

    auto setGoInnKeeper = [&]() -> bool
    {
        WorldPosition pos = SelectRandomInnKeeperPos();
        if (!pos)
            return false;
        info.innKeeperPos = pos;
        info.lastGoInnKeeper = now;
        info.status = NewRpgStatus::GO_INNKEEPER;
        return true;
    };

    switch (info.status)
    {
        case NewRpgStatus::IDLE:
        {
            if (botAI->HasStrategy("rpg quest", BOT_STATE_NON_COMBAT) ||
                botAI->HasStrategy("rpg vendor", BOT_STATE_NON_COMBAT) ||
                botAI->HasStrategy("rpg guild", BOT_STATE_NON_COMBAT) ||
                botAI->HasStrategy("rpg player", BOT_STATE_NON_COMBAT) ||
                botAI->HasStrategy("rpg craft", BOT_STATE_NON_COMBAT))
            {
                if (trySetNearNpc())
                    return true;
            }

            if (botAI->HasStrategy("rpg explore", BOT_STATE_NON_COMBAT) ||
                botAI->HasStrategy("rpg jump", BOT_STATE_NON_COMBAT))
            {
                return setNearRandom();
            }

            if (botAI->HasStrategy("rpg maintenance", BOT_STATE_NON_COMBAT))
            {
                if (setGoInnKeeper())
                    return true;
                return setRest();
            }

            if (botAI->HasStrategy("rpg bg", BOT_STATE_NON_COMBAT))
            {
                if (setGoGrind())
                    return true;
            }

            uint32 roll = urand(1, 100);
            if (roll <= 30 && trySetNearNpc())
                return true;
            if (roll <= 55 && setGoInnKeeper())
                return true;
            if (roll <= 90 && setGoGrind())
                return true;

            return setRest();
        }
        case NewRpgStatus::GO_GRIND:
        {
            if (info.grindPos && bot->GetExactDist(info.grindPos) < 10.0f)
            {
                info.grindPos = WorldPosition();
                return setNearRandom();
            }
            break;
        }
        case NewRpgStatus::GO_INNKEEPER:
        {
            if (info.innKeeperPos && bot->GetExactDist(info.innKeeperPos) < 10.0f)
            {
                info.innKeeperPos = WorldPosition();
                if (trySetNearNpc())
                    return true;
                return setNearRandom();
            }
            break;
        }
        case NewRpgStatus::NEAR_RANDOM:
        {
            if (info.lastNearRandom + statusNearRandomDuration < now)
            {
                info.status = NewRpgStatus::IDLE;
                return true;
            }
            break;
        }
        case NewRpgStatus::NEAR_NPC:
        {
            if (!info.npcPos || !info.npcPos.GetUnit())
            {
                info.status = NewRpgStatus::IDLE;
                info.npcPos = GuidPosition();
                info.lastReachNpc = 0;
                return true;
            }

            if (info.lastNearNpc + statusNearNpcDuration < now)
            {
                info.status = NewRpgStatus::IDLE;
                info.npcPos = GuidPosition();
                info.lastReachNpc = 0;
                return true;
            }
            break;
        }
        case NewRpgStatus::REST:
        {
            if (info.lastRest + statusRestDuration < now)
            {
                info.status = NewRpgStatus::IDLE;
                bot->SetStandState(UNIT_STAND_STATE_STAND);
                return true;
            }
            break;
        }
        default:
            break;
    }

    return false;
}

WorldPosition NewRpgStatusUpdateAction::SelectRandomGrindPos()
{
    if (!bot)
        return WorldPosition();
    WorldPosition dest(bot);
    if (!dest.GetReachableRandomPointOnGround(bot, 2000.0f, true))
        return WorldPosition();
    return dest;
}

WorldPosition NewRpgStatusUpdateAction::SelectRandomInnKeeperPos()
{
    if (!bot)
        return WorldPosition();
    WorldPosition dest(bot);
    if (!dest.GetReachableRandomPointOnGround(bot, 800.0f, true))
        return WorldPosition();
    return dest;
}

bool NewRpgGoFarAwayPosAction::MoveFarTo(WorldPosition dest)
{
    if (!dest || bot->GetMapId() != dest.GetMapId())
        return false;

    return MoveTo(dest.GetMapId(), dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(),
        false, false, false, true);
}

bool NewRpgGoGrindAction::Execute(Event event)
{
    NewRpgInfo& info = botAI->GetRpgInfo();
    if (!info.grindPos)
        return false;
    return MoveFarTo(info.grindPos);
}

bool NewRpgGoInnKeeperAction::Execute(Event event)
{
    NewRpgInfo& info = botAI->GetRpgInfo();
    if (!info.innKeeperPos)
        return false;
    return MoveFarTo(info.innKeeperPos);
}

bool NewRpgMoveRandomAction::Execute(Event event)
{
    float distance = 10.0f + rand_norm() * moveStep;
    Map* map = bot->GetMap();
    const float x = bot->GetPositionX();
    const float y = bot->GetPositionY();
    const float z = bot->GetPositionZ();
    int attempts = 5;
    while (--attempts)
    {
        float angle = (float)rand_norm() * 2 * static_cast<float>(M_PI);
        float dx = x + distance * cos(angle);
        float dy = y + distance * sin(angle);
        float dz = z;
        if (!map->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(),
            dx, dy, dz))
            continue;

        if (map->IsInWater(bot->GetPhaseMask(), dx, dy, dz, nullptr))
            continue;

        bool moved = MoveTo(bot->GetMapId(), dx, dy, dz, false, false, false, true);
        if (moved)
            return true;
    }

    return false;
}

bool NewRpgMoveNpcAction::Execute(Event event)
{
    NewRpgInfo& info = botAI->GetRpgInfo();

    if (!info.npcPos)
    {
        info.npcPos = FindNearbyNpc(bot, sPlayerbotAIConfig->reactDistance);
        info.lastReachNpc = 0;
        if (!info.npcPos)
            return false;
    }

    Unit* unit = info.npcPos.GetUnit();
    if (!unit || !unit->IsAlive())
    {
        info.npcPos = GuidPosition();
        info.lastReachNpc = 0;
        return false;
    }

    if (bot->GetDistance(unit) <= INTERACTION_DISTANCE)
    {
        if (!info.lastReachNpc)
        {
            info.lastReachNpc = getMSTime();
            return true;
        }

        if (info.lastReachNpc + stayTime > getMSTime())
            return false;

        info.npcPos = GuidPosition();
        info.lastReachNpc = 0;
        return true;
    }

    return MoveTo(unit, INTERACTION_DISTANCE);
}
