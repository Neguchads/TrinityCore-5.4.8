/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "PvpTriggers.h"

#include "BattleGroundTactics.h"
#include "BattlegroundAV.h"
#include "BattlegroundEY.h"
#include "BattlegroundMgr.h"
#include "BattlegroundWS.h"
#include "Playerbots.h"
#include "ServerFacade.h"

bool EnemyPlayerNear::IsActive()
{
    return AI_VALUE(Unit*, "enemy player target");
}

bool PlayerHasNoFlag::IsActive()
{
    if (botAI->GetBot()->InBattleground())
    {
        if (botAI->GetBot()->GetBattlegroundTypeId() == BattlegroundTypeId::BATTLEGROUND_WS)
        {
            BattlegroundWS* bg = static_cast<BattlegroundWS*>(botAI->GetBot()->GetBattleground());
            uint32 enemyTeam = bg->GetOtherTeam(bot->GetBGTeam());
            if (bg->GetFlagState(enemyTeam) != BG_WS_FLAG_STATE_ON_PLAYER)
                return true;

            if (bot->GetGUID() == bg->GetFlagPickerGUID(TEAM_ALLIANCE) ||
                bot->GetGUID() == bg->GetFlagPickerGUID(TEAM_HORDE))
            {
                return false;
            }
            return true;
        }
        return false;
    }

    return false;
}

bool PlayerIsInBattleground::IsActive()
{
    return botAI->GetBot()->InBattleground();
}

bool BgWaitingTrigger::IsActive()
{
    if (bot->InBattleground())
    {
        if (bot->GetBattleground() && bot->GetBattleground()->GetStatus() == STATUS_WAIT_JOIN)
            return true;
    }

    return false;
}

bool BgActiveTrigger::IsActive()
{
    if (bot->InBattleground())
    {
        if (bot->GetBattleground() && bot->GetBattleground()->GetStatus() == STATUS_IN_PROGRESS)
            return true;
    }

    return false;
}

bool BgInviteActiveTrigger::IsActive()
{
    if (bot->InBattleground() || !bot->InBattlegroundQueue())
        return false;

    for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
    {
        BattlegroundQueueTypeId queueTypeId = bot->GetBattlegroundQueueTypeId(i);
        if (queueTypeId == BATTLEGROUND_QUEUE_NONE)
            continue;

        BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);

        GroupQueueInfo ginfo;
        if (bgQueue.GetPlayerGroupInfoData(bot->GetGUID(), &ginfo))
        {
            if (ginfo.IsInvitedToBGInstanceGUID && ginfo.RemoveInviteTime)
                return true;
        }
    }

    return false;
}

bool InsideBGTrigger::IsActive()
{
    return bot->InBattleground() && bot->GetBattleground();
}

bool PlayerIsInBattlegroundWithoutFlag::IsActive()
{
    if (botAI->GetBot()->InBattleground())
    {
        if (botAI->GetBot()->GetBattlegroundTypeId() == BattlegroundTypeId::BATTLEGROUND_WS)
        {
            BattlegroundWS* bg = static_cast<BattlegroundWS*>(botAI->GetBot()->GetBattleground());
            uint32 enemyTeam = bg->GetOtherTeam(bot->GetBGTeam());
            if (bg->GetFlagState(enemyTeam) != BG_WS_FLAG_STATE_ON_PLAYER)
                return true;

            if (bot->GetGUID() == bg->GetFlagPickerGUID(TEAM_ALLIANCE) ||
                bot->GetGUID() == bg->GetFlagPickerGUID(TEAM_ALLIANCE))
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

bool PlayerHasFlag::IsActive()
{
    return IsCapturingFlag(bot);
}

bool PlayerHasFlag::IsCapturingFlag(Player* bot)
{
    if (bot->InBattleground())
    {
        if (bot->GetBattlegroundTypeId() == BATTLEGROUND_WS)
        {
            BattlegroundWS* bg = static_cast<BattlegroundWS*>(bot->GetBattleground());
            if (bot->GetGUID() == bg->GetFlagPickerGUID(TEAM_ALLIANCE))
            {
                if (bg->GetFlagPickerGUID(TEAM_HORDE))
                {
                    if (GameObject* go = bg->GetBGObject(BG_WS_OBJECT_H_FLAG))
                        return bot->GetDistance(go) > 36.0f;
                }
                return true;
            }
            if (bot->GetGUID() == bg->GetFlagPickerGUID(TEAM_HORDE))
            {
                if (bg->GetFlagPickerGUID(TEAM_ALLIANCE))
                {
                    if (GameObject* go = bg->GetBGObject(BG_WS_OBJECT_A_FLAG))
                        return bot->GetDistance(go) > 36.0f;
                }
                return true;
            }
            return false;
        }

        if (bot->GetBattlegroundTypeId() == BATTLEGROUND_EY)
        {
            BattlegroundEY* bg = static_cast<BattlegroundEY*>(bot->GetBattleground());

            if (bot->GetGUID() == bg->GetFlagPickerGUID())
            {
                uint32 controlledBases = 0;
                for (uint8 point = 0; point < EY_POINTS_MAX; ++point)
                {
                    if (bg->GetCapturePointInfo(point)._ownerTeamId == bot->GetBGTeamId())
                        controlledBases++;
                }

                if (controlledBases == 0)
                    return false;

                return bot->GetGUID() == bg->GetFlagPickerGUID();
            }
        }

        return false;
    }

    return false;
}

bool TeamHasFlag::IsActive()
{
    if (!botAI->GetBot()->InBattleground())
        return false;

    if (botAI->GetBot()->GetBattlegroundTypeId() != BattlegroundTypeId::BATTLEGROUND_WS)
        return false;

    BattlegroundWS* bg = static_cast<BattlegroundWS*>(botAI->GetBot()->GetBattleground());

    ObjectGuid botGuid = bot->GetGUID();
    uint32 team = bot->GetBGTeam();
    uint32 enemyTeam = bg->GetOtherTeam(team);

    if (botGuid == bg->GetFlagPickerGUID(TEAM_ALLIANCE) || botGuid == bg->GetFlagPickerGUID(TEAM_HORDE))
        return false;

    bool ownTeamHasFlag = bg->GetFlagState(enemyTeam) == BG_WS_FLAG_STATE_ON_PLAYER;
    bool enemyTeamHasFlag = bg->GetFlagState(team) == BG_WS_FLAG_STATE_ON_PLAYER;

    return ownTeamHasFlag && !enemyTeamHasFlag;
}

bool EnemyTeamHasFlag::IsActive()
{
    if (botAI->GetBot()->InBattleground())
    {
        if (botAI->GetBot()->GetBattlegroundTypeId() == BattlegroundTypeId::BATTLEGROUND_WS)
        {
            BattlegroundWS* bg = static_cast<BattlegroundWS*>(botAI->GetBot()->GetBattleground());

            if (bot->GetTeamId() == TEAM_HORDE)
            {
                if (!bg->GetFlagPickerGUID(TEAM_HORDE).IsEmpty())
                    return true;
            }
            else
            {
                if (!bg->GetFlagPickerGUID(TEAM_ALLIANCE).IsEmpty())
                    return true;
            }
        }

        return false;
    }

    return false;
}

bool EnemyFlagCarrierNear::IsActive()
{
    Unit* carrier = AI_VALUE(Unit*, "enemy flag carrier");

    if (!carrier || !sServerFacade->IsDistanceLessOrEqualThan(sServerFacade->GetDistance2d(bot, carrier), 100.f))
        return false;

    Unit* nearbyEnemy = AI_VALUE(Unit*, "enemy player target");

    if (nearbyEnemy)
    {
        float distToFC = sServerFacade->GetDistance2d(bot, carrier);
        float distToEnemy = sServerFacade->GetDistance2d(bot, nearbyEnemy);

        if (distToEnemy + 15.0f < distToFC)
            return false;
    }

    return true;
}

bool TeamFlagCarrierNear::IsActive()
{
    if (bot->GetBattlegroundTypeId() == BATTLEGROUND_WS)
    {
        BattlegroundWS* bg = dynamic_cast<BattlegroundWS*>(bot->GetBattleground());
        if (bg)
        {
            bool bothFlagsNotAtBase =
                bg->GetFlagState(ALLIANCE) != BG_WS_FLAG_STATE_ON_BASE &&
                bg->GetFlagState(HORDE) != BG_WS_FLAG_STATE_ON_BASE;

            if (bothFlagsNotAtBase)
                return false;
        }
    }

    Unit* carrier = AI_VALUE(Unit*, "team flag carrier");
    return carrier && sServerFacade->IsDistanceLessOrEqualThan(sServerFacade->GetDistance2d(bot, carrier), 200.f);
}

bool PlayerWantsInBattlegroundTrigger::IsActive()
{
    if (bot->InBattleground())
        return false;

    if (bot->GetBattleground() && bot->GetBattleground()->GetStatus() == STATUS_WAIT_JOIN)
        return false;

    if (bot->GetBattleground() && bot->GetBattleground()->GetStatus() == STATUS_IN_PROGRESS)
        return false;

    if (!bot->CanJoinToBattleground(nullptr))
        return false;

    return true;
}

bool VehicleNearTrigger::IsActive()
{
    GuidVector npcs = AI_VALUE(GuidVector, "nearest vehicles");
    return !npcs.empty();
}

bool InVehicleTrigger::IsActive()
{
    return botAI->IsInVehicle();
}

bool AllianceNoSnowfallGY::IsActive()
{
    if (!bot || bot->GetTeamId() != TEAM_ALLIANCE)
        return false;

    Battleground* bg = bot->GetBattleground();
    if (bg && BGTactics::GetBotStrategyForTeam(bg, TEAM_ALLIANCE) != AV_STRATEGY_BALANCED)
        return false;

    float botX = bot->GetPositionX();
    if (botX <= -562.0f)
        return false;

    if (bot->GetBattlegroundTypeId() != BATTLEGROUND_AV)
        return false;

    if (BattlegroundAV* av = dynamic_cast<BattlegroundAV*>(bg))
    {
        const BG_AV_NodeInfo& snowfall = av->GetAVNodeInfo(BG_AV_NODES_SNOWFALL_GRAVE);
        return snowfall.Owner != ALLIANCE;
    }

    return false;
}
