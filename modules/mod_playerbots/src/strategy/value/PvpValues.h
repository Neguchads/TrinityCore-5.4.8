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
#ifndef _PLAYERBOT_PVPVALUES_H
#define _PLAYERBOT_PVPVALUES_H

#include "Value.h"
#include "DBCEnums.h"

class PlayerbotAI;
class Unit;

class BgTypeValue : public ManualSetValue<uint32>
{
public:
    BgTypeValue(PlayerbotAI* botAI) : ManualSetValue<uint32>(botAI, 0, "bg type") {}
};

class ArenaTypeValue : public ManualSetValue<uint32>
{
public:
    ArenaTypeValue(PlayerbotAI* botAI) : ManualSetValue<uint32>(botAI, 0, "arena type") {}
};

class BgRoleValue : public ManualSetValue<uint32>
{
public:
    BgRoleValue(PlayerbotAI* botAI) : ManualSetValue<uint32>(botAI, 0, "bg role") {}
};

class FlagCarrierValue : public UnitCalculatedValue
{
public:
    FlagCarrierValue(PlayerbotAI* botAI, bool sameTeam = false, bool ignoreRange = false)
        : UnitCalculatedValue(botAI), sameTeam(sameTeam), ignoreRange(ignoreRange)
    {
    }

    Unit* Calculate() override;

private:
    bool sameTeam;
    bool ignoreRange;
};

class RpgBgTypeValue : public CalculatedValue<BattlegroundTypeId>
{
public:
    RpgBgTypeValue(PlayerbotAI* botAI) : CalculatedValue<BattlegroundTypeId>(botAI, "rpg bg type") {}

    BattlegroundTypeId Calculate() override;
};

#endif
