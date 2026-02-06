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

#ifndef _PLAYERBOT_ADDLOOTACTION_H
#define _PLAYERBOT_ADDLOOTACTION_H

#include "Action.h"

class ObjectGuid;
class PlayerbotAI;

class AddLootAction : public Action
{
public:
    AddLootAction(PlayerbotAI* botAI) : Action(botAI, "add loot") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class AddAllLootAction : public Action
{
public:
    AddAllLootAction(PlayerbotAI* botAI, std::string const name = "add all loot") : Action(botAI, name) {}
    bool Execute(Event event) override;
    bool isUseful() override;

protected:
    virtual bool AddLoot(ObjectGuid guid);
};

class AddGatheringLootAction : public AddAllLootAction
{
public:
    AddGatheringLootAction(PlayerbotAI* botAI) : AddAllLootAction(botAI, "add gathering loot") {}

protected:
    bool AddLoot(ObjectGuid guid) override;
};

#endif
