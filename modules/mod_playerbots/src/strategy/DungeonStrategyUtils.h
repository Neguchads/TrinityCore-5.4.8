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

#ifndef _PLAYERBOT_DUNGEONUTILS_H
#define _PLAYERBOT_DUNGEONUTILS_H

template <class T>
inline const T& DUNGEON_MODE(Player* bot, const T& normal5, const T& heroic10)
{
    switch (bot->GetMap()->GetDifficulty())
    {
        case DUNGEON_DIFFICULTY_NORMAL:
            return normal5;
        case DUNGEON_DIFFICULTY_HEROIC:
            return heroic10;
        default:
            break;
    }

    return heroic10;
}

#endif
