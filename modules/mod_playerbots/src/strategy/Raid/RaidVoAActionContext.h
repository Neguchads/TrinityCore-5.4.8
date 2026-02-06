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

#ifndef _PLAYERBOT_RAIDVOAACTIONCONTEXT_H
#define _PLAYERBOT_RAIDVOAACTIONCONTEXT_H

#include "Action.h"
#include "NamedObjectContext.h"
#include "RaidVoAActions.h"
#include "PlayerbotAI.h"

class RaidVoAActionContext : public NamedObjectContext<Action>
{
public:
    RaidVoAActionContext()
    {
        creators["emalon mark boss action"] = &RaidVoAActionContext::emalon_mark_boss_action;
        creators["emalon lighting nova action"] = &RaidVoAActionContext::emalon_lighting_nova_action;
        creators["emalon overcharge action"] = &RaidVoAActionContext::emalon_overcharge_action;
        creators["emalon fall from floor action"] = &RaidVoAActionContext::emalon_fall_from_floor_action;
        creators["emalon nature resistance action"] = &RaidVoAActionContext::emalon_nature_resistance_action;
        creators["koralon fire resistance action"] = &RaidVoAActionContext::koralon_fire_resistance_action;
    }

private:
    static Action* emalon_mark_boss_action(PlayerbotAI* ai) { return new EmalonMarkBossAction(ai); }
    static Action* emalon_lighting_nova_action(PlayerbotAI* ai) { return new EmalonLightingNovaAction(ai); }
    static Action* emalon_overcharge_action(PlayerbotAI* ai) { return new EmalonOverchargeAction(ai); }
    static Action* emalon_fall_from_floor_action(PlayerbotAI* ai) { return new EmalonFallFromFloorAction(ai); }
    static Action* emalon_nature_resistance_action(PlayerbotAI* ai) { return new BossNatureResistanceAction(ai, "emalon the storm watcher"); }
    static Action* koralon_fire_resistance_action(PlayerbotAI* ai) { return new BossFireResistanceAction(ai, "koralon the flame watcher"); }
};

#endif
