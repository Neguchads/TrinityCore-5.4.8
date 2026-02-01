/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
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
