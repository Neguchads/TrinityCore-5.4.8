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

#ifndef _PLAYERBOT_LFGACTIONS_H
#define _PLAYERBOT_LFGACTIONS_H

#include "Action.h"

class PlayerbotAI;
class WorldPacket;

class LfgJoinAction : public Action
{
public:
    LfgJoinAction(PlayerbotAI* botAI, std::string const name = "lfg join") : Action(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;

protected:
    bool JoinLfg();
    uint32 GetRoles();
};

class LfgAcceptAction : public LfgJoinAction
{
public:
    LfgAcceptAction(PlayerbotAI* botAI) : LfgJoinAction(botAI, "lfg accept") {}

    bool Execute(Event event) override;
    bool isUseful() override { return true; }

private:
    uint32 ExtractProposalId(WorldPacket& packet, uint32& queueId) const;
};

class LfgRoleCheckAction : public LfgJoinAction
{
public:
    LfgRoleCheckAction(PlayerbotAI* botAI) : LfgJoinAction(botAI, "lfg role check") {}

    bool Execute(Event event) override;
    bool isUseful() override { return true; }
};

class LfgLeaveAction : public Action
{
public:
    LfgLeaveAction(PlayerbotAI* botAI) : Action(botAI, "lfg leave") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class LfgTeleportAction : public Action
{
public:
    LfgTeleportAction(PlayerbotAI* botAI) : Action(botAI, "lfg teleport") {}

    bool Execute(Event event) override;
};

#endif
