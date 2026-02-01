/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "PvpValues.h"

#include "BattlegroundEY.h"
#include "BattlegroundWS.h"
#include "Playerbots.h"

Unit* FlagCarrierValue::Calculate()
{
    Unit* carrier = nullptr;

    if (!botAI->GetBot()->InBattleground())
        return nullptr;

    if (botAI->GetBot()->GetBattlegroundTypeId() == BattlegroundTypeId::BATTLEGROUND_WS)
    {
        BattlegroundWS* bg = static_cast<BattlegroundWS*>(botAI->GetBot()->GetBattleground());
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
