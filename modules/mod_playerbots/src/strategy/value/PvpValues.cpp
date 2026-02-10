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
#include "PvpValues.h"

#include "BattlegroundEY.h"
#include "BattlegroundMgr.h"
#include "BattlegroundTP.h"
#include "BattlegroundWS.h"
#include "Playerbots.h"
#include "ServerFacade.h"

Unit* FlagCarrierValue::Calculate()
{
    Unit* carrier = nullptr;

    if (!botAI->GetBot()->InBattleground())
        return nullptr;

    BattlegroundTypeId bgTypeId = botAI->GetBot()->GetBattlegroundTypeId();

    if (bgTypeId == BattlegroundTypeId::BATTLEGROUND_WS || bgTypeId == BattlegroundTypeId::BATTLEGROUND_TP)
    {
        Battleground* bg = botAI->GetBot()->GetBattleground();
        if (!bg)
            return nullptr;

        if ((!sameTeam && bot->GetTeamId() == TEAM_HORDE) || (sameTeam && bot->GetTeamId() == TEAM_ALLIANCE))
        {
            if (!bg->GetFlagPickerGUID(TEAM_HORDE).IsEmpty())
                carrier = ObjectAccessor::GetPlayer(bg->GetBgMap(), bg->GetFlagPickerGUID(TEAM_HORDE));
        }

        if ((!sameTeam && bot->GetTeamId() == TEAM_ALLIANCE) || (sameTeam && bot->GetTeamId() == TEAM_HORDE))
        {
            if (!bg->GetFlagPickerGUID(TEAM_ALLIANCE).IsEmpty())
                carrier = ObjectAccessor::GetPlayer(bg->GetBgMap(), bg->GetFlagPickerGUID(TEAM_ALLIANCE));
        }

        if (carrier)
            return (ignoreRange || bot->IsWithinDistInMap(carrier, sPlayerbotAIConfig->sightDistance)) ? carrier : nullptr;
    }

    if (botAI->GetBot()->GetBattlegroundTypeId() == BATTLEGROUND_EY)
    {
        BattlegroundEY* bg = static_cast<BattlegroundEY*>(botAI->GetBot()->GetBattleground());
        if (!bg)
            return nullptr;

        if (bg->GetFlagPickerGUID().IsEmpty())
            return nullptr;

        Player* fc = ObjectAccessor::GetPlayer(bg->GetBgMap(), bg->GetFlagPickerGUID());
        if (!fc)
            return nullptr;

        if (!sameTeam && (fc->GetTeamId() != bot->GetTeamId()))
            carrier = fc;

        if (sameTeam && (fc->GetTeamId() == bot->GetTeamId()))
            carrier = fc;

        if (carrier)
            return (ignoreRange || bot->IsWithinDistInMap(carrier, sPlayerbotAIConfig->sightDistance)) ? carrier : nullptr;
    }

    return carrier;
}

BattlegroundTypeId RpgBgTypeValue::Calculate()
{
    GuidPosition guidPosition = AI_VALUE(GuidPosition, "rpg target");
    if (!guidPosition)
        return BATTLEGROUND_TYPE_NONE;

    if (!guidPosition.HasNpcFlag(UNIT_NPC_FLAG_BATTLEMASTER))
        return BATTLEGROUND_TYPE_NONE;

    BattlegroundTypeId bgTypeId = sBattlegroundMgr->GetBattleMasterBG(guidPosition.GetEntry());
    if (bgTypeId == BATTLEGROUND_TYPE_NONE)
        return BATTLEGROUND_TYPE_NONE;

    Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg)
        return BATTLEGROUND_TYPE_NONE;

    if (bot->GetLevel() < bg->GetMinLevel())
        return BATTLEGROUND_TYPE_NONE;

    BattlegroundQueueTypeId queueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, 0);
    if (bot->InBattlegroundQueueForBattlegroundQueueType(queueTypeId))
        return BATTLEGROUND_TYPE_NONE;

    return bgTypeId;
}
