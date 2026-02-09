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

#ifndef _PLAYERBOT_CUSTOMSTRATEGYEDITACTION_H
#define _PLAYERBOT_CUSTOMSTRATEGYEDITACTION_H

#include "Action.h"

class CustomStrategyEditAction : public Action
{
public:
    CustomStrategyEditAction(PlayerbotAI* botAI) : Action(botAI, "cs") {}

    bool Execute(Event event) override;

private:
    bool PrintHelp();
    bool PrintActionLine(uint32 idx, std::string const command);
    bool Print(std::string const name);
    bool Edit(std::string const name, uint32 idx, std::string const command);
};

#endif
