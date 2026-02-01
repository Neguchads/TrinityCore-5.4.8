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

#ifndef _PLAYERBOT_NEWPLAYERNEARBYVALUE_H
#define _PLAYERBOT_NEWPLAYERNEARBYVALUE_H

#include "ObjectGuid.h"
#include "Value.h"

class PlayerbotAI;

class NewPlayerNearbyValue : public CalculatedValue<ObjectGuid>
{
public:
    NewPlayerNearbyValue(PlayerbotAI* botAI) : CalculatedValue<ObjectGuid>(botAI, "new player nearby") {}

    ObjectGuid Calculate() override;
};

class AlreadySeenPlayersValue : public ManualSetValue<GuidSet&>
{
public:
    AlreadySeenPlayersValue(PlayerbotAI* botAI) : ManualSetValue<GuidSet&>(botAI, data, "already seen players") {}

    GuidSet data;
};

#endif
