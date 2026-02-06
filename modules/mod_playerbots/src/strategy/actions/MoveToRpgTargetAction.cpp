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

#include "MoveToRpgTargetAction.h"

#include "ChatHelper.h"
#include "ChooseRpgTargetAction.h"
#include "Event.h"
#include "LastMovementValue.h"
#include "Playerbots.h"
#include "TravelMgr.h"

#include <cmath>

bool MoveToRpgTargetAction::Execute(Event /*event*/)
{
    GuidPosition guidP = AI_VALUE(GuidPosition, "rpg target");
    Unit* unit = botAI->GetUnit(guidP);
    if (unit && !unit->IsInWorld())
        return false;

    GameObject* go = botAI->GetGameObject(guidP);
    if (go && !go->IsInWorld())
        return false;

    WorldObject* wo = nullptr;
    if (unit)
        wo = unit;
    else if (go)
        wo = go;
    else
        return false;

    if (botAI->HasStrategy("debug rpg", BOT_STATE_NON_COMBAT) && guidP.GetWorldObject())
    {
        std::ostringstream out;
        out << "Heading to: " << ChatHelper::FormatWorldobject(guidP.GetWorldObject());
        botAI->TellMasterNoFacing(out);
    }

    if (guidP.IsPlayer())
    {
        Player* player = guidP.GetPlayer();

        if (player && GET_PLAYERBOT_AI(player))
        {
            GuidPosition guidPP = PAI_VALUE(GuidPosition, "rpg target");
            if (guidPP.IsPlayer())
            {
                AI_VALUE(GuidSet&, "ignore rpg target").insert(AI_VALUE(GuidPosition, "rpg target"));
                RESET_AI_VALUE(GuidPosition, "rpg target");
                return false;
            }
        }
    }

    if ((unit && unit->isMoving() && !urand(0, 20)) || !ChooseRpgTargetAction::isFollowValid(bot, wo) ||
        guidP.distance(bot) > sPlayerbotAIConfig->reactDistance * 2 || !urand(0, 50))
    {
        AI_VALUE(GuidSet&, "ignore rpg target").insert(AI_VALUE(GuidPosition, "rpg target"));
        RESET_AI_VALUE(GuidPosition, "rpg target");
        return false;
    }

    float x = wo->GetPositionX();
    float y = wo->GetPositionY();
    float z = wo->GetPositionZ();
    float mapId = wo->GetMapId();

    float angle = 0.f;
    float distance = 1.0f;
    if (bot->IsWithinLOS(x, y, z))
    {
        if (!unit || !unit->isMoving())
            angle = wo->GetAngle(bot) + (M_PI * irand(-25, 25) / 100.0f);
        else
            angle = wo->GetOrientation() + (M_PI * irand(-25, 25) / 100.0f);

        distance = frand(0.5f, 1.0f);
    }
    else
        angle = 2 * M_PI * urand(0, 100) / 100.0f;

    x += cos(angle) * INTERACTION_DISTANCE * distance;
    y += sin(angle) * INTERACTION_DISTANCE * distance;
    bool exact = true;
    if (!wo->GetMap()->CheckCollisionAndGetValidCoords(wo, wo->GetPositionX(), wo->GetPositionY(), wo->GetPositionZ(), x,
                                                       y, z))
    {
        x = wo->GetPositionX();
        y = wo->GetPositionY();
        z = wo->GetPositionZ();
        exact = false;
    }

    bool couldMove = MoveTo(mapId, x, y, z, false, false, false, exact);

    if (!couldMove && WorldPosition(mapId, x, y, z).distance(bot) > INTERACTION_DISTANCE)
    {
        AI_VALUE(GuidSet&, "ignore rpg target").insert(AI_VALUE(GuidPosition, "rpg target"));
        RESET_AI_VALUE(GuidPosition, "rpg target");
    }

    return couldMove;
}

bool MoveToRpgTargetAction::isUseful()
{
    GuidPosition guidP = AI_VALUE(GuidPosition, "rpg target");
    if (!guidP)
        return false;

    WorldObject* wo = guidP.GetWorldObject();
    if (!wo)
        return false;

    TravelTarget* travelTarget = AI_VALUE(TravelTarget*, "travel target");
    if (travelTarget && travelTarget->isTraveling() && ChooseRpgTargetAction::isFollowValid(bot, *travelTarget->getPosition()))
        return false;

    if (guidP.distance(bot) < INTERACTION_DISTANCE)
        return false;

    if (!ChooseRpgTargetAction::isFollowValid(bot, wo))
        return false;

    if (!AI_VALUE(bool, "can move around"))
        return false;

    return true;
}
