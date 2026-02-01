/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "LfgActions.h"

#include "DBCStores.h"
#include "Event.h"
#include "LFGMgr.h"
#include "Group.h"
#include "ObjectAccessor.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotSpec.h"
#include "Playerbots.h"
#include "WorldPacket.h"

using namespace lfg;

namespace
{
    bool GroupHasRealPlayer(Group* group)
    {
        if (!group)
            return false;

        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (!member)
                continue;

            PlayerbotAI* memberAI = GET_PLAYERBOT_AI(member);
            if (!memberAI || memberAI->IsRealPlayer())
                return true;
        }

        return false;
    }

    bool HasRealPlayerInLfgQueue()
    {
        for (auto const& managerPair : sLFGMgr->GetQueueManagers())
        {
            QueueManager const& manager = managerPair.second;
            for (auto const& dataPair : manager.GetQueuerData())
            {
                for (auto const& playerGuid : dataPair.second.Players)
                {
                    Player* player = ObjectAccessor::FindConnectedPlayer(playerGuid);
                    if (!player)
                        continue;

                    if (!player->GetSession() || !player->GetSession()->IsBot())
                        return true;

                    PlayerbotAI* botAI = GET_PLAYERBOT_AI(player);
                    if (!botAI || botAI->IsRealPlayer())
                        return true;
                }
            }
        }

        return false;
    }
}

bool LfgJoinAction::Execute(Event event)
{
    return JoinLfg();
}

bool LfgJoinAction::isUseful()
{
    if (!sPlayerbotAIConfig->randomBotJoinLfg)
        return false;

    if (bot->GetLevel() < 15)
        return false;

    if (botAI->HasActivePlayerMaster())
        return false;

    if (bot->GetGroup() && bot->GetGroup()->GetLeaderGUID() != bot->GetGUID())
        return false;

    if (GroupHasRealPlayer(bot->GetGroup()))
        return false;

    if (!HasRealPlayerInLfgQueue())
        return false;

    if (bot->IsBeingTeleported())
        return false;

    if (bot->InBattleground() || bot->InBattlegroundQueue())
        return false;

    if (bot->isDead())
        return false;

    if (!sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    if (Map* map = bot->GetMap())
    {
        if (map->Instanceable())
            return false;
    }

    Group* group = bot->GetGroup();
    ObjectGuid guid = group ? group->GetGUID() : bot->GetGUID();
    uint32 queueId = sLFGMgr->GetActiveQueueId(guid);
    if (queueId && sLFGMgr->GetState(guid, queueId) != LFG_STATE_NONE)
        return false;

    if (sRandomPlayerbotMgr->LfgDungeons[bot->GetTeamId()].empty())
        return false;

    return true;
}

uint32 LfgJoinAction::GetRoles()
{
    if (PlayerBotSpec::IsTank(bot, true))
        return PLAYER_ROLE_TANK;
    if (PlayerBotSpec::IsHeal(bot, true))
        return PLAYER_ROLE_HEALER;
    return PLAYER_ROLE_DAMAGE;
}

bool LfgJoinAction::JoinLfg()
{
    Group* group = bot->GetGroup();
    ObjectGuid guid = group ? group->GetGUID() : bot->GetGUID();
    uint32 queueId = sLFGMgr->GetActiveQueueId(guid);
    if (queueId && sLFGMgr->GetState(guid, queueId) != LFG_STATE_NONE)
        return false;

    LfgDungeonSet list;

    std::vector<uint32> dungeons = sRandomPlayerbotMgr->LfgDungeons[bot->GetTeamId()];
    if (dungeons.empty())
        return false;

    for (uint32 dungeonId : dungeons)
    {
        LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(dungeonId);
        if (!dungeon)
            continue;

        uint32 botLevel = bot->GetLevel();
        if (dungeon->minlevel && (botLevel < dungeon->minlevel || botLevel > dungeon->maxlevel))
            continue;

        list.insert(dungeon->ID);
    }

    if (list.empty())
        return false;

    uint32 roleMask = GetRoles();
    std::string comment = std::to_string(PlayerbotAI::GetMixedGearScore(bot, false, false));

    sLFGMgr->JoinLfg(bot, LfgRoles(roleMask), list, comment);
    return true;
}

bool LfgRoleCheckAction::Execute(Event event)
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    uint32 roleMask = GetRoles();
    sLFGMgr->UpdateRoleCheck(group->GetGUID(), bot->GetGUID(), roleMask);
    return true;
}

uint32 LfgAcceptAction::ExtractProposalId(WorldPacket& packet, uint32& queueId) const
{
    queueId = 0;
    if (packet.empty())
        return 0;

    packet.rpos(0);

    ObjectGuid guid1;
    ObjectGuid guid2;

    guid2[6] = packet.ReadBit();
    guid2[0] = packet.ReadBit();
    guid1[1] = packet.ReadBit();
    guid1[7] = packet.ReadBit();
    guid1[5] = packet.ReadBit();
    guid2[5] = packet.ReadBit();
    guid1[4] = packet.ReadBit();
    packet.ReadBit(); // silent
    guid2[2] = packet.ReadBit();
    guid1[6] = packet.ReadBit();
    guid2[3] = packet.ReadBit();
    guid2[7] = packet.ReadBit();
    guid1[3] = packet.ReadBit();

    uint32 players = packet.ReadBits(21);
    for (uint32 i = 0; i < players; ++i)
    {
        packet.ReadBit(); // in dungeon
        packet.ReadBit(); // is you
        packet.ReadBit(); // answered
        packet.ReadBit(); // accept
        packet.ReadBit(); // same group
    }

    guid1[2] = packet.ReadBit();
    guid2[4] = packet.ReadBit();
    packet.ReadBit(); // unk
    guid1[0] = packet.ReadBit();
    guid2[1] = packet.ReadBit();

    packet.FlushBits();

    packet.ReadByteSeq(guid2[1]);
    packet.ReadByteSeq(guid1[4]);
    packet.ReadByteSeq(guid2[4]);
    packet.ReadByteSeq(guid1[7]);
    packet.ReadByteSeq(guid1[2]);
    packet.ReadByteSeq(guid1[0]);

    uint32 dungeonEntry;
    uint8 state;
    packet >> dungeonEntry;
    packet >> state;
    packet >> queueId;

    packet.ReadByteSeq(guid2[6]);

    uint32 proposalId = 0;
    packet >> proposalId;
    return proposalId;
}

bool LfgAcceptAction::Execute(Event event)
{
    uint32 proposalId = AI_VALUE(uint32, "lfg proposal");
    uint32 queueId = 0;

    if (!proposalId && !event.getPacket().empty())
    {
        WorldPacket p(event.getPacket());
        proposalId = ExtractProposalId(p, queueId);
        if (proposalId)
            botAI->GetAiObjectContext()->GetValue<uint32>("lfg proposal")->Set(proposalId);
    }

    if (!proposalId)
        return false;

    bool accept = !(bot->IsInCombat() || bot->isDead());
    if (accept)
        bot->ClearUnitState(UNIT_STATE_ALL_STATE);
    sLFGMgr->UpdateProposal(proposalId, bot->GetGUID(), accept);

    botAI->GetAiObjectContext()->GetValue<uint32>("lfg proposal")->Set(0);
    if (sRandomPlayerbotMgr->IsRandomBot(bot) && !bot->GetGroup())
    {
        sRandomPlayerbotMgr->Refresh(bot);
        botAI->ResetStrategies();
    }

    botAI->Reset();
    return true;
}

bool LfgLeaveAction::Execute(Event event)
{
    Group* group = bot->GetGroup();
    ObjectGuid guid = bot->GetGUID();
    ObjectGuid gguid = group ? group->GetGUID() : guid;

    uint32 queueId = sLFGMgr->GetActiveQueueId(gguid);
    if (!queueId)
        queueId = sLFGMgr->GetActiveQueueId(guid);
    if (!queueId)
        return false;

    sLFGMgr->LeaveLfg(gguid, queueId);
    return true;
}

bool LfgLeaveAction::isUseful()
{
    Group* group = bot->GetGroup();
    ObjectGuid guid = bot->GetGUID();
    ObjectGuid gguid = group ? group->GetGUID() : guid;

    if (group && group->GetLeaderGUID() != guid)
        return false;

    uint32 queueId = sLFGMgr->GetActiveQueueId(gguid);
    if (!queueId)
        queueId = sLFGMgr->GetActiveQueueId(guid);
    if (!queueId)
        return false;

    LfgState state = sLFGMgr->GetState(gguid, queueId);
    if (state == LFG_STATE_NONE || state >= LFG_STATE_DUNGEON || state == LFG_STATE_PROPOSAL ||
        state == LFG_STATE_ROLECHECK)
        return false;

    if (!sPlayerbotAIConfig->randomBotJoinLfg)
        return true;

    if (!HasRealPlayerInLfgQueue())
        return true;

    if (!sRandomPlayerbotMgr->LfgDungeons[bot->GetTeamId()].empty())
        return false;

    return true;
}

bool LfgTeleportAction::Execute(Event event)
{
    bool out = false;
    if (!event.getPacket().empty())
    {
        WorldPacket packet(event.getPacket());
        out = packet.ReadBit();
    }

    bot->ClearUnitState(UNIT_STATE_ALL_STATE);
    sLFGMgr->TeleportPlayer(bot, out, true);
    return true;
}
