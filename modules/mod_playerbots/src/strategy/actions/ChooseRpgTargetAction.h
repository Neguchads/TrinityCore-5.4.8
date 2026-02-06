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

#ifndef _PLAYERBOT_CHOOSERPGTARGETACTION_H
#define _PLAYERBOT_CHOOSERPGTARGETACTION_H

#include <unordered_map>

#include "ObjectGuid.h"
#include "RpgAction.h"

class GuidPosition;
class Player;
class PlayerbotAI;
class WorldObject;
class WorldPosition;

class ChooseRpgTargetAction : public Action
{
public:
    ChooseRpgTargetAction(PlayerbotAI* botAI, std::string const name = "choose rpg target") : Action(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;

    static bool isFollowValid(Player* bot, WorldObject* target);
    static bool isFollowValid(Player* bot, WorldPosition pos);

private:
    float getMaxRelevance(GuidPosition guidP);
    bool HasSameTarget(ObjectGuid guid, uint32 max, GuidVector const& nearGuids);

    std::unordered_map<ObjectGuid, std::string> rpgActionReason;
};

class ClearRpgTargetAction : public ChooseRpgTargetAction
{
public:
    ClearRpgTargetAction(PlayerbotAI* botAI) : ChooseRpgTargetAction(botAI, "clear rpg target") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

#endif
