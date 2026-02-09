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

#ifndef _PLAYERBOT_LOOTSTRATEGYVALUE_H
#define _PLAYERBOT_LOOTSTRATEGYVALUE_H

#include "Value.h"

class LootStrategy;
class PlayerbotAI;

class LootStrategyValue : public ManualSetValue<LootStrategy*>
{
public:
    LootStrategyValue(PlayerbotAI* botAI, std::string const name = "loot strategy")
        : ManualSetValue<LootStrategy*>(botAI, normal, name)
    {
    }
    ~LootStrategyValue() override;

    std::string const Save() override;
    bool Load(std::string const value) override;

    static LootStrategy* normal;
    static LootStrategy* gray;
    static LootStrategy* all;
    static LootStrategy* disenchant;
    static LootStrategy* instance(std::string const name);
};

#endif
