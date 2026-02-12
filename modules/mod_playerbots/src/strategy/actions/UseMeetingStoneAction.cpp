#include "UseMeetingStoneAction.h"

#include <cmath>

#include "Event.h"
#include "Pet.h"
#include "Player.h"
#include "Playerbots.h"

bool SummonAction::Execute(Event event)
{
    Player* summoner = event.getOwner() ? event.getOwner() : GetMaster();
    if (!summoner)
        return false;

    return Teleport(summoner, bot);
}

bool SummonAction::Teleport(Player* summoner, Player* player)
{
    if (!summoner || !player || summoner == player)
        return false;

    if (summoner->IsBeingTeleported() || player->IsBeingTeleported())
        return false;

    if (player->GetVehicle())
    {
        botAI->TellError("You cannot summon me while I'm on a vehicle");
        return false;
    }

    float followAngle = GetFollowAngle();
    float const maxAngle = followAngle + static_cast<float>(M_PI);
    for (float angle = followAngle - static_cast<float>(M_PI); angle <= maxAngle; angle += ANGLE_45_DEG)
    {
        uint32 mapId = summoner->GetMapId();
        float x = summoner->GetPositionX() + std::cos(angle) * sPlayerbotAIConfig->followDistance;
        float y = summoner->GetPositionY() + std::sin(angle) * sPlayerbotAIConfig->followDistance;
        float z = summoner->GetPositionZ();
        summoner->UpdateGroundPositionZ(x, y, z);

        if (!summoner->IsWithinLOS(x, y, z + player->GetCollisionHeight()))
        {
            x = summoner->GetPositionX();
            y = summoner->GetPositionY();
            z = summoner->GetPositionZ();
        }

        if (!summoner->IsWithinLOS(x, y, z + player->GetCollisionHeight()))
            continue;

        player->GetMotionMaster()->Clear();
        player->TeleportTo(mapId, x, y, z, summoner->GetOrientation());
        if (Pet* pet = player->GetPet())
            pet->NearTeleportTo(x, y, z, player->GetOrientation());
        return true;
    }

    botAI->TellError("Not enough place to summon");
    return false;
}
