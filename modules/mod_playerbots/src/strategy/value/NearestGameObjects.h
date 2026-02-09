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

#ifndef _PLAYERBOT_NEARESTGAMEOBJECTS_H
#define _PLAYERBOT_NEARESTGAMEOBJECTS_H

#include "GameObject.h"
#include "PlayerbotAIConfig.h"
#include "Value.h"

class PlayerbotAI;

class AnyGameObjectInObjectRangeCheck
{
public:
    AnyGameObjectInObjectRangeCheck(WorldObject const* obj, float range) : i_obj(obj), i_range(range) {}
    WorldObject const& GetFocusObject() const { return *i_obj; }
    bool operator()(GameObject* go)
    {
        if (go && i_obj->IsWithinDistInMap(go, i_range) && go->isSpawned() && go->GetGOInfo())
            return true;

        return false;
    }

private:
    WorldObject const* i_obj;
    float i_range;
};

class NearestGameObjects : public ObjectGuidListCalculatedValue
{
public:
    NearestGameObjects(PlayerbotAI* botAI, float range = sPlayerbotAIConfig->sightDistance, bool ignoreLos = false,
        std::string const name = "nearest game objects")
        : ObjectGuidListCalculatedValue(botAI, name, 1 * 1000), range(range), ignoreLos(ignoreLos)
    {
    }

protected:
    GuidVector Calculate() override;

private:
    float range;
    bool ignoreLos;
};

#endif
