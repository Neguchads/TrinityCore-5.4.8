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

#include "AlwaysLootListValue.h"

#include <cstdlib>
#include <sstream>
#include <vector>

#include "Helper.h"
#include "Playerbots.h"

std::string const AlwaysLootListValue::Save()
{
    std::ostringstream out;
    bool first = true;
    for (auto const& itemId : value)
    {
        if (!first)
            out << ",";
        else
            first = false;

        out << itemId;
    }

    return out.str();
}

bool AlwaysLootListValue::Load(std::string const text)
{
    value.clear();

    std::vector<std::string> ss = split(text, ',');
    for (auto const& entry : ss)
        value.insert(uint32(atoi(entry.c_str())));

    return true;
}
