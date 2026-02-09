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

#ifndef _PLAYERBOT_ALWAYSLOOTLISTVALUE_H
#define _PLAYERBOT_ALWAYSLOOTLISTVALUE_H

#include <set>
#include <string>

#include "Value.h"

class PlayerbotAI;

class AlwaysLootListValue : public ManualSetValue<std::set<uint32>&>
{
public:
    AlwaysLootListValue(PlayerbotAI* botAI, std::string const name = "always loot list")
        : ManualSetValue<std::set<uint32>&>(botAI, list, name)
    {
    }

    std::string const Save() override;
    bool Load(std::string const value) override;

private:
    std::set<uint32> list;
};

#endif
