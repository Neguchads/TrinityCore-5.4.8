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
#include "BattleGroundJoinAction.h"

#include "DatabaseEnv.h"
#include "ArenaTeam.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BattlegroundQueue.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Event.h"
#include "LFGMgr.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Opcodes.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "WorldPacket.h"

bool BGJoinAction::Execute(Event event)
{
    if (bgList.empty())
        return false;

    BattlegroundQueueTypeId queueTypeId = BattlegroundQueueTypeId(bgList[urand(0, bgList.size() - 1)]);
    if (uint8 arenaType = BattlegroundMgr::BGArenaType(queueTypeId))
    {
        if (!PrepareArenaGroup(arenaType))
            return false;
    }
    return JoinQueue(queueTypeId);
}

bool BGJoinAction::canJoinBg(BattlegroundQueueTypeId queueTypeId, BattlegroundBracketId bracketId)
{
    BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(queueTypeId);
    uint8 arenaType = BattlegroundMgr::BGArenaType(queueTypeId);

    if (bot->InBattlegroundQueueForBattlegroundQueueType(queueTypeId))
        return false;

    if (!bot->GetBGAccessByLevel(bgTypeId))
        return false;

    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg)
        return false;

    if (!bot->CanJoinToBattleground(bg))
        return false;

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), bot->GetLevel());
    if (!bracketEntry)
        return false;

    if (arenaType)
    {
        Group* group = bot->GetGroup();
        if (!group)
        {
            // Allow random bots to attempt arena queue without a group; group will be formed in PrepareArenaGroup().
            if (!sRandomPlayerbotMgr->IsRandomBot(bot))
                return false;
        }
        else
        {
            if (group->GetLeaderGUID() != bot->GetGUID())
                return false;

            if (group->GetMembersCount() != arenaType)
                return false;
        }
    }

    return bracketEntry->GetBracketId() == bracketId;
}

bool BGJoinAction::shouldJoinBg(BattlegroundQueueTypeId queueTypeId, BattlegroundBracketId bracketId)
{
    BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(queueTypeId);
    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg)
        return false;

    auto& bgInfo = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId];
    uint8 arenaType = BattlegroundMgr::BGArenaType(queueTypeId);
    uint32 targetQueues = arenaType ? (bgInfo.activeRatedArenaQueue + bgInfo.activeSkirmishArenaQueue) : bgInfo.activeBgQueue;
    if (sPlayerbotAIConfig->randomBotAutoJoinBG)
        targetQueues += arenaType ? (bgInfo.ratedArenaInstanceCount + bgInfo.skirmishArenaInstanceCount) : bgInfo.bgInstanceCount;
    if (!targetQueues)
        return false;

    if (arenaType)
        return (bgInfo.ratedArenaPlayerCount + bgInfo.skirmishArenaPlayerCount +
            bgInfo.ratedArenaBotCount + bgInfo.skirmishArenaBotCount) < arenaType * 2 * targetQueues;

    uint32 maxPerTeam = bg->GetMaxPlayersPerTeam();
    if (bot->GetTeamId() == TEAM_ALLIANCE)
        return (bgInfo.bgAlliancePlayerCount + bgInfo.bgAllianceBotCount) < maxPerTeam * targetQueues;

    return (bgInfo.bgHordePlayerCount + bgInfo.bgHordeBotCount) < maxPerTeam * targetQueues;
}

bool BGJoinAction::isUseful()
{
    if (!sPlayerbotAIConfig->randomBotJoinBG)
        return false;

    if (bot->InBattleground() || bot->InBattlegroundQueue())
        return false;

    if (bot->GetLevel() < 10)
        return false;

    if (botAI->HasActivePlayerMaster())
        return false;

    if (bot->GetGroup() && !bot->GetGroup()->IsLeader(bot->GetGUID()))
        return false;

    if (bot->IsInCombat())
        return false;

    if (!bot->HasFreeBattlegroundQueueId())
        return false;

    bgList.clear();

    for (uint32 bracket = BG_BRACKET_ID_FIRST; bracket < MAX_BATTLEGROUND_BRACKETS; ++bracket)
    {
        for (uint32 queueType = BATTLEGROUND_QUEUE_AV; queueType < MAX_BATTLEGROUND_QUEUE_TYPES; ++queueType)
        {
            BattlegroundQueueTypeId queueTypeId = BattlegroundQueueTypeId(queueType);
            BattlegroundBracketId bracketId = BattlegroundBracketId(bracket);

            if (!canJoinBg(queueTypeId, bracketId))
                continue;

            if (shouldJoinBg(queueTypeId, bracketId))
                bgList.push_back(queueTypeId);
        }
    }

    return !bgList.empty();
}

bool BGJoinAction::JoinQueue(uint32 type)
{
    if (!bot || (!bot->IsInWorld() && !bot->IsBeingTeleported()) || bot->InBattleground())
        return false;

    BattlegroundQueueTypeId queueTypeId = BattlegroundQueueTypeId(type);
    BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(queueTypeId);
    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg)
        return false;

    uint8 arenaType = BattlegroundMgr::BGArenaType(queueTypeId);
    if (arenaType)
    {
        Group* group = bot->GetGroup();
        if (!group || group->GetLeaderGUID() != bot->GetGUID())
            return false;

        if (group->GetMembersCount() != arenaType)
            return false;

        if (bot->InBattlegroundQueueForBattlegroundQueueType(queueTypeId))
            return false;

        if (!bot->GetBGAccessByLevel(bgTypeId))
            return false;

        if (bot->IsUsingLfg(true))
            return false;

        PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), bot->GetLevel());
        if (!bracketEntry)
            return false;

        auto& bgInfo = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketEntry->GetBracketId()];
        bool hasRated = bgInfo.activeRatedArenaQueue || bgInfo.ratedArenaInstanceCount;
        bool hasSkirmish = bgInfo.activeSkirmishArenaQueue || bgInfo.skirmishArenaInstanceCount;
        bool joinRated = hasRated || !hasSkirmish;

        uint8 arenaSlot = ArenaTeam::GetSlotByType(arenaType);
        uint32 arenaRating = 0;
        uint32 matchmakerRating = 0;
        if (joinRated)
        {
            RatedPvpSlot slot = RatedPvpSlot(arenaSlot);
            ArenaTeam team{ slot, group };
            arenaRating = team.GetRating();
            matchmakerRating = team.GetMatchmakerRating();
            if (arenaRating <= 0)
                arenaRating = 1;
        }

        BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);
        GroupJoinBattlegroundResult err = group->CanJoinBattlegroundQueue(bg, queueTypeId, arenaType, arenaType, joinRated, arenaSlot);

        uint32 avgTime = 0;
        GroupQueueInfo* ginfo = nullptr;

        if (!err)
        {
            ginfo = bgQueue.AddGroup(bot, group, bgTypeId, bracketEntry, arenaType, joinRated, false, arenaRating, matchmakerRating);
            avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        }

        for (auto&& member : *group)
        {
            if (err)
            {
                WorldPacket data;
                sBattlegroundMgr->BuildStatusFailedPacket(&data, bg, bot, 0, err);
                member->GetSession()->SendPacket(&data);
                continue;
            }

            uint32 queueSlot = member->AddBattlegroundQueueId(queueTypeId);
            member->AddBattlegroundQueueJoinTime(bgTypeId, ginfo->JoinTime);

            WorldPacket data;
            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, member, queueSlot, STATUS_WAIT_QUEUE, avgTime, ginfo->JoinTime, arenaType);
            member->GetSession()->SendPacket(&data);
        }

        sBattlegroundMgr->ScheduleQueueUpdate(matchmakerRating, arenaType, queueTypeId, bgTypeId, bracketEntry->GetBracketId());

        TC_LOG_INFO("playerbots", "Bot %u %s:%u <%s> queued %s arena queueType=%u arenaType=%u bracket=%u",
            bot->GetGUID().GetCounter(),
            bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H",
            bot->GetLevel(),
            bot->GetName().c_str(),
            joinRated ? "rated" : "skirmish",
            queueTypeId,
            arenaType,
            bracketEntry->GetBracketId());

        return !err;
    }

    if (!bot->CanJoinToBattleground(bg))
        return false;

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), bot->GetLevel());
    if (!bracketEntry)
        return false;

    if (bot->InBattlegroundQueueForBattlegroundQueueType(queueTypeId))
        return false;

    if (!bot->GetBGAccessByLevel(bgTypeId))
        return false;

    if (bot->IsUsingLfg(true))
        return false;

    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);
    GroupQueueInfo* ginfo = bgQueue.AddGroup(bot, nullptr, bgTypeId, bracketEntry, 0, false, false, 0, 0);

    uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
    uint32 queueSlot = bot->AddBattlegroundQueueId(queueTypeId);
    bot->AddBattlegroundQueueJoinTime(bgTypeId, ginfo->JoinTime);

    WorldPacket data;
    sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, bot, queueSlot, STATUS_WAIT_QUEUE, avgTime,
        ginfo->JoinTime, ginfo->ArenaType);
    bot->GetSession()->SendPacket(&data);

    TC_LOG_INFO("playerbots", "Bot %u %s:%u <%s> queued BG queueType=%u bgType=%u bracket=%u",
        bot->GetGUID().GetCounter(),
        bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H",
        bot->GetLevel(),
        bot->GetName().c_str(),
        queueTypeId,
        bgTypeId,
        bracketEntry->GetBracketId());

    sBattlegroundMgr->ScheduleQueueUpdate(0, 0, queueTypeId, bgTypeId, bracketEntry->GetBracketId());
    return true;
}

bool BGJoinAction::PrepareArenaGroup(uint8 arenaType)
{
    if (!arenaType)
        return true;

    Group* group = bot->GetGroup();
    if (group)
    {
        if (group->GetLeaderGUID() == bot->GetGUID() && group->GetMembersCount() >= arenaType)
            return true;
        return false;
    }

    if (!sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
    if (!bg)
        return false;

    PvPDifficultyEntry const* leaderBracket = GetBattlegroundBracketByLevel(bg->GetMapId(), bot->GetLevel());
    if (!leaderBracket)
        return false;

    Group* newGroup = new Group();
    if (!newGroup->Create(bot))
    {
        delete newGroup;
        return false;
    }
    sGroupMgr->AddGroup(newGroup);

    for (PlayerBotMap::const_iterator it = sRandomPlayerbotMgr->GetPlayerBotsBegin();
         it != sRandomPlayerbotMgr->GetPlayerBotsEnd(); ++it)
    {
        if (newGroup->GetMembersCount() >= arenaType)
            break;

        Player* member = it->second;
        if (!member || member == bot || !member->IsInWorld())
            continue;

        if (!sRandomPlayerbotMgr->IsRandomBot(member))
            continue;

        PlayerbotAI* memberAI = GET_PLAYERBOT_AI(member);
        if (!memberAI || memberAI->HasActivePlayerMaster())
            continue;

        if (member->IsInCombat() || member->IsBeingTeleported() || member->isDead())
            continue;

        if (member->InBattleground() || member->InBattlegroundQueue())
            continue;

        if (member->GetGroup())
            continue;

        if (!member->GetBGAccessByLevel(BATTLEGROUND_AA))
            continue;

        PvPDifficultyEntry const* memberBracket = GetBattlegroundBracketByLevel(bg->GetMapId(), member->GetLevel());
        if (!memberBracket || memberBracket->GetBracketId() != leaderBracket->GetBracketId())
            continue;

        if (!newGroup->AddMember(member))
            continue;

        memberAI->Reset();
        member->RemoveAurasWithInterruptFlags(
            SpellAuraInterruptFlags(AURA_INTERRUPT_FLAG_TELEPORTED | AURA_INTERRUPT_FLAG_CHANGE_MAP), 0, nullptr);
        member->TeleportTo(bot->GetMapId(), bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), 0.0f);
    }

    if (newGroup->GetMembersCount() >= arenaType)
        return true;

    newGroup->Disband(true);
    return false;
}

bool FreeBGJoinAction::shouldJoinBg(BattlegroundQueueTypeId queueTypeId, BattlegroundBracketId bracketId)
{
    return BGJoinAction::shouldJoinBg(queueTypeId, bracketId);
}

bool BGLeaveAction::Execute(Event event)
{
    if (!bot->InBattlegroundQueue() && !bot->InBattleground())
        return false;

    WorldPacket packet(CMSG_BATTLEFIELD_LEAVE);
    bot->GetSession()->HandleBattlefieldLeaveOpcode(packet);
    return true;
}

bool BGStatusAction::JoinInvitedBattleground(uint8 queueSlot, BattlegroundQueueTypeId queueTypeId, GroupQueueInfo const& ginfo)
{
    if (!bot->IsInvitedForBattlegroundQueueType(queueTypeId))
        return false;

    BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(queueTypeId);
    Battleground* bg = sBattlegroundMgr->GetBattleground(
        ginfo.IsInvitedToBGInstanceGUID, bgTypeId == BATTLEGROUND_AA ? BATTLEGROUND_TYPE_NONE : bgTypeId);
    if (!bg)
    {
        TC_LOG_INFO("playerbots", "Bot %u %s:%u <%s> invite missing BG instance=%u bgType=%u",
            bot->GetGUID().GetCounter(),
            bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H",
            bot->GetLevel(),
            bot->GetName().c_str(),
            ginfo.IsInvitedToBGInstanceGUID,
            bgTypeId);
        return false;
    }

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), bot->GetLevel());
    if (!bracketEntry)
    {
        TC_LOG_INFO("playerbots", "Bot %u %s:%u <%s> invite invalid bracket map=%u level=%u",
            bot->GetGUID().GetCounter(),
            bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H",
            bot->GetLevel(),
            bot->GetName().c_str(),
            bg->GetMapId(),
            bot->GetLevel());
        return false;
    }

    if (!bot->InBattleground())
        bot->SetBattlegroundEntryPoint();

    if (!bot->IsAlive())
    {
        bot->ResurrectPlayer(1.0f);
        bot->SpawnCorpseBones();
    }

    if (bot->IsInFlight())
    {
        bot->GetMotionMaster()->MovementExpired();
        bot->CleanupAfterTaxiFlight();
    }

    WorldPacket data;
    sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, bot, queueSlot, STATUS_IN_PROGRESS,
        bot->GetBattlegroundQueueJoinTime(bgTypeId), bg->GetElapsedTime(), bg->GetArenaType());
    bot->GetSession()->SendPacket(&data);

    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);
    bgQueue.RemovePlayer(bot->GetGUID(), false);

    if (Battleground* currentBg = bot->GetBattleground())
        currentBg->RemovePlayerAtLeave(bot->GetGUID(), false, true);

    bot->SetBattlegroundId(bg->GetInstanceID(), bgTypeId);
    bot->SetBGTeam(ginfo.Team);

    // Ensure BG strategies replace follow behavior when entering the battleground.
    botAI->ResetStrategies();

    sLFGMgr->RemovePlayerQueuesOnPartyFound(bot->GetGUID());
    sBattlegroundMgr->SendToBattleground(bot, ginfo.IsInvitedToBGInstanceGUID, bgTypeId);

    TC_LOG_INFO("playerbots", "Bot %u %s:%u <%s> accepted BG invite instance=%u bgType=%u queueType=%u",
        bot->GetGUID().GetCounter(),
        bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H",
        bot->GetLevel(),
        bot->GetName().c_str(),
        ginfo.IsInvitedToBGInstanceGUID,
        bgTypeId,
        queueTypeId);
    return true;
}

bool BGStatusAction::LeaveQueue(uint8 queueSlot, BattlegroundQueueTypeId queueTypeId, GroupQueueInfo const& ginfo)
{
    BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(queueTypeId);
    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg)
        return false;

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), bot->GetLevel());
    if (!bracketEntry)
        return false;

    WorldPacket data;
    sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, bot, queueSlot, STATUS_NONE,
        bot->GetBattlegroundQueueJoinTime(bgTypeId), 0, 0);
    bot->GetSession()->SendPacket(&data);

    bot->RemoveBattlegroundQueueId(queueTypeId);
    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);
    bgQueue.RemovePlayer(bot->GetGUID(), true, true);
    if (!ginfo.ArenaType)
        sBattlegroundMgr->ScheduleQueueUpdate(ginfo.ArenaMatchmakerRating, ginfo.ArenaType, queueTypeId, bgTypeId,
            bracketEntry->GetBracketId());

    return true;
}

bool BGStatusAction::isUseful()
{
    return bot->InBattlegroundQueue() || bot->InBattleground();
}

bool BGStatusAction::Execute(Event event)
{
    Battleground* bg = bot->GetBattleground();
    if (bg && bg->GetStatus() == STATUS_WAIT_LEAVE)
    {
        WorldPacket packet(CMSG_BATTLEFIELD_LEAVE);
        bot->GetSession()->HandleBattlefieldLeaveOpcode(packet);
        return true;
    }

    if (!bot->InBattlegroundQueue())
        return false;

    for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
    {
        BattlegroundQueueTypeId queueTypeId = bot->GetBattlegroundQueueTypeId(i);
        if (queueTypeId == BATTLEGROUND_QUEUE_NONE)
            continue;

        BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);
        GroupQueueInfo ginfo;
        if (!bgQueue.GetPlayerGroupInfoData(bot->GetGUID(), &ginfo))
            continue;

        if (ginfo.IsInvitedToBGInstanceGUID && !bot->InBattleground())
        {
            TC_LOG_INFO("playerbots", "Bot %u %s:%u <%s> got BG invite queueType=%u instance=%u",
                bot->GetGUID().GetCounter(),
                bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H",
                bot->GetLevel(),
                bot->GetName().c_str(),
                queueTypeId,
                ginfo.IsInvitedToBGInstanceGUID);
            return JoinInvitedBattleground(i, queueTypeId, ginfo);
        }

        BattlegroundTypeId bgTypeId = BattlegroundMgr::BGTemplateId(queueTypeId);
        Battleground* bgTemplate = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
        if (!bgTemplate)
            continue;

        PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bgTemplate->GetMapId(), bot->GetLevel());
        if (!bracketEntry)
            continue;

        BattlegroundBracketId bracketId = bracketEntry->GetBracketId();
        auto& bgInfo = sRandomPlayerbotMgr->BattlegroundData[queueTypeId][bracketId];
        uint8 arenaType = BattlegroundMgr::BGArenaType(queueTypeId);
        bool activeQueue = arenaType ? (bgInfo.activeRatedArenaQueue > 0 || sPlayerbotAIConfig->randomBotAutoJoinBG)
                                      : (bgInfo.activeBgQueue > 0 || sPlayerbotAIConfig->randomBotAutoJoinBG);

        if (!activeQueue)
        {
            TC_LOG_INFO("playerbots", "Bot %u %s:%u <%s> leaving BG queueType=%u (inactive queue)",
                bot->GetGUID().GetCounter(),
                bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H",
                bot->GetLevel(),
                bot->GetName().c_str(),
                queueTypeId);
            return LeaveQueue(i, queueTypeId, ginfo);
        }
    }

    return false;
}

bool BGStatusCheckAction::Execute(Event event)
{
    if (bot->IsBeingTeleported())
        return false;

    WorldPacket packet(CMSG_BATTLEFIELD_STATUS);
    bot->GetSession()->HandleBattlefieldStatusOpcode(packet);
    return true;
}

bool BGStatusCheckAction::isUseful()
{
    return bot->InBattlegroundQueue();
}

bool BGStrategyCheckAction::Execute(Event event)
{
    bool insideBg = bot->InBattleground() && bot->GetBattleground();

    if (!insideBg && botAI->HasStrategy("battleground", BOT_STATE_NON_COMBAT))
    {
        botAI->ResetStrategies();
        return true;
    }

    if (insideBg && !botAI->HasStrategy("battleground", BOT_STATE_NON_COMBAT))
    {
        botAI->ResetStrategies();
        return false;
    }

    return false;
}
