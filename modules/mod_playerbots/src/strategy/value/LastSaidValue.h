/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#ifndef _PLAYERBOT_LASTSAIDVALUE_H
#define _PLAYERBOT_LASTSAIDVALUE_H

#include <ctime>

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

class LastSaidValue : public ManualSetValue<time_t>, public Qualified
{
public:
    LastSaidValue(PlayerbotAI* botAI) : ManualSetValue<time_t>(botAI, time(nullptr) - 120, "last said") {}
};

class LastEmoteValue : public ManualSetValue<time_t>, public Qualified
{
public:
    LastEmoteValue(PlayerbotAI* botAI) : ManualSetValue<time_t>(botAI, time(nullptr) - 120, "last emote") {}
};

#endif
