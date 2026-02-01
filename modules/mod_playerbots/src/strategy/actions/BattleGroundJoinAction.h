/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_BATTLEGROUNDJOINACTION_H
#define _PLAYERBOT_BATTLEGROUNDJOINACTION_H

#include "Action.h"
#include "DBCEnums.h"
#include "SharedDefines.h"

#include <vector>

class PlayerbotAI;
struct GroupQueueInfo;

class BGJoinAction : public Action
{
public:
    BGJoinAction(PlayerbotAI* botAI, std::string const name = "bg join") : Action(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
    bool canJoinBg(BattlegroundQueueTypeId queueTypeId, BattlegroundBracketId bracketId);
    virtual bool shouldJoinBg(BattlegroundQueueTypeId queueTypeId, BattlegroundBracketId bracketId);

protected:
    bool JoinQueue(uint32 type);
    bool PrepareArenaGroup(uint8 arenaType);
    std::vector<uint32> bgList;
};

class FreeBGJoinAction : public BGJoinAction
{
public:
    FreeBGJoinAction(PlayerbotAI* botAI, std::string const name = "free bg join") : BGJoinAction(botAI, name) {}

    bool shouldJoinBg(BattlegroundQueueTypeId queueTypeId, BattlegroundBracketId bracketId) override;
};

class BGLeaveAction : public Action
{
public:
    BGLeaveAction(PlayerbotAI* botAI, std::string const name = "bg leave") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

class BGStatusAction : public Action
{
public:
    BGStatusAction(PlayerbotAI* botAI) : Action(botAI, "bg status") {}

    bool Execute(Event event) override;
    bool isUseful() override;

private:
    bool JoinInvitedBattleground(uint8 queueSlot, BattlegroundQueueTypeId queueTypeId, GroupQueueInfo const& ginfo);
    bool LeaveQueue(uint8 queueSlot, BattlegroundQueueTypeId queueTypeId, GroupQueueInfo const& ginfo);
};

class BGStatusCheckAction : public Action
{
public:
    BGStatusCheckAction(PlayerbotAI* botAI, std::string const name = "bg status check") : Action(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class BGStrategyCheckAction : public Action
{
public:
    BGStrategyCheckAction(PlayerbotAI* botAI, std::string const name = "bg strategy check") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

#endif
