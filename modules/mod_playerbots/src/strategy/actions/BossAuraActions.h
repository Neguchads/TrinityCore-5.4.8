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
#ifndef _PLAYERBOT_BOSSAURAACTION_H
#define _PLAYERBOT_BOSSAURAACTION_H

#include <string>

#include "Action.h"

class PlayerbotAI;

class BossFireResistanceAction : public Action
{
public:
    BossFireResistanceAction(PlayerbotAI* botAI, std::string const bossName)
        : Action(botAI, bossName + " fire resistance action"), bossName(bossName)
    {
    }
    bool Execute(Event event) override;
    bool isUseful() override;

private:
    std::string bossName;
};

class BossFrostResistanceAction : public Action
{
public:
    BossFrostResistanceAction(PlayerbotAI* botAI, std::string const bossName)
        : Action(botAI, bossName + " frost resistance action"), bossName(bossName)
    {
    }
    bool Execute(Event event) override;
    bool isUseful() override;

private:
    std::string bossName;
};

class BossNatureResistanceAction : public Action
{
public:
    BossNatureResistanceAction(PlayerbotAI* botAI, std::string const bossName)
        : Action(botAI, bossName + " nature resistance action"), bossName(bossName)
    {
    }
    bool Execute(Event event) override;
    bool isUseful() override;

private:
    std::string bossName;
};

class BossShadowResistanceAction : public Action
{
public:
    BossShadowResistanceAction(PlayerbotAI* botAI, std::string const bossName)
        : Action(botAI, bossName + " shadow resistance action"), bossName(bossName)
    {
    }
    bool Execute(Event event) override;
    bool isUseful() override;

private:
    std::string bossName;
};

#endif
