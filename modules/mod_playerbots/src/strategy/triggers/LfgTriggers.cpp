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

#include "LfgTriggers.h"

#include "LFGMgr.h"
#include "Playerbots.h"

bool LfgProposalActiveTrigger::IsActive()
{
    return AI_VALUE(uint32, "lfg proposal");
}

bool LfgTeleportInTrigger::IsActive()
{
    if (!bot || bot->IsBeingTeleported() || bot->IsInCombat() || bot->isDead())
        return false;

    if (bot->InBattleground() || bot->InBattlegroundQueue())
        return false;

    ObjectGuid guid = bot->GetGUID();
    ObjectGuid gguid = bot->GetGroup() ? bot->GetGroup()->GetGUID() : guid;

    uint32 queueId = sLFGMgr->GetActiveQueueId(gguid);
    if (!queueId)
        queueId = sLFGMgr->GetActiveQueueId(guid);
    if (!queueId)
        return false;

    lfg::LfgState state = sLFGMgr->GetState(gguid, queueId);
    if (state != lfg::LFG_STATE_DUNGEON)
        return false;

    if (sLFGMgr->InLfgDungeonMap(guid, bot->GetMapId(), bot->GetDungeonDifficulty()))
        return false;

    return true;
}
