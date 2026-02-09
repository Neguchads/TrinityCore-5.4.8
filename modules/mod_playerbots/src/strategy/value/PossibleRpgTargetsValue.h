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

#ifndef _PLAYERBOT_POSSIBLERPGTARGETSVALUE_H
#define _PLAYERBOT_POSSIBLERPGTARGETSVALUE_H

#include "NearestGameObjects.h"
#include "NearestUnitsValue.h"
#include "PlayerbotAIConfig.h"
#include "SharedDefines.h"

class PlayerbotAI;

class PossibleRpgTargetsValue : public NearestUnitsValue
{
public:
    PossibleRpgTargetsValue(PlayerbotAI* botAI, float range = 70.0f);

    static std::vector<uint32> allowedNpcFlags;

protected:
    void FindUnits(std::list<Unit*>& targets) override;
    bool AcceptUnit(Unit* unit) override;
};

class PossibleNewRpgTargetsValue : public NearestUnitsValue
{
public:
    PossibleNewRpgTargetsValue(PlayerbotAI* botAI, float range = 150.0f);

    static std::vector<uint32> allowedNpcFlags;
    GuidVector Calculate() override;

protected:
    void FindUnits(std::list<Unit*>& targets) override;
    bool AcceptUnit(Unit* unit) override;
};

class PossibleNewRpgGameObjectsValue : public ObjectGuidListCalculatedValue
{
public:
    PossibleNewRpgGameObjectsValue(PlayerbotAI* botAI, float range = 150.0f, bool ignoreLos = true)
        : ObjectGuidListCalculatedValue(botAI, "possible new rpg game objects"), range(range), ignoreLos(ignoreLos)
    {
        if (allowedGOFlags.empty())
            allowedGOFlags.push_back(GAMEOBJECT_TYPE_QUESTGIVER);
    }

    static std::vector<GameobjectTypes> allowedGOFlags;
    GuidVector Calculate() override;

private:
    float range;
    bool ignoreLos;
};

#endif
