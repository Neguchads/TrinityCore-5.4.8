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
#include "BossAuraActions.h"

#include <initializer_list>

#include "BossAuraTriggers.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"

namespace
{
    uint32 GetBestKnownSpell(Player* bot, std::initializer_list<uint32> spellIds)
    {
        for (uint32 spellId : spellIds)
        {
            if (bot->HasActiveSpell(spellId))
                return spellId;
        }
        return 0;
    }

    bool CastBestKnownSpell(PlayerbotAI* botAI, Player* bot, std::initializer_list<uint32> spellIds)
    {
        uint32 spellId = GetBestKnownSpell(bot, spellIds);
        if (!spellId)
            return false;

        if (!botAI->CanCastSpell(spellId, bot))
            return false;

        return botAI->CastSpell(spellId, bot);
    }
}

bool BossFireResistanceAction::isUseful()
{
    BossFireResistanceTrigger bossFireResistanceTrigger(botAI, bossName);
    return bossFireResistanceTrigger.IsActive();
}

bool BossFireResistanceAction::Execute(Event event)
{
    return CastBestKnownSpell(botAI, bot,
        { SPELL_FIRE_RESISTANCE_AURA_RANK_5, SPELL_FIRE_RESISTANCE_AURA_RANK_4, SPELL_FIRE_RESISTANCE_AURA_RANK_3,
          SPELL_FIRE_RESISTANCE_AURA_RANK_2, SPELL_FIRE_RESISTANCE_AURA_RANK_1 });
}

bool BossFrostResistanceAction::isUseful()
{
    BossFrostResistanceTrigger bossFrostResistanceTrigger(botAI, bossName);
    return bossFrostResistanceTrigger.IsActive();
}

bool BossFrostResistanceAction::Execute(Event event)
{
    return CastBestKnownSpell(botAI, bot,
        { SPELL_FROST_RESISTANCE_AURA_RANK_5, SPELL_FROST_RESISTANCE_AURA_RANK_4, SPELL_FROST_RESISTANCE_AURA_RANK_3,
          SPELL_FROST_RESISTANCE_AURA_RANK_2, SPELL_FROST_RESISTANCE_AURA_RANK_1 });
}

bool BossNatureResistanceAction::isUseful()
{
    BossNatureResistanceTrigger bossNatureResistanceTrigger(botAI, bossName);
    return bossNatureResistanceTrigger.IsActive();
}

bool BossNatureResistanceAction::Execute(Event event)
{
    return CastBestKnownSpell(botAI, bot,
        { SPELL_ASPECT_OF_THE_WILD_RANK_4, SPELL_ASPECT_OF_THE_WILD_RANK_3, SPELL_ASPECT_OF_THE_WILD_RANK_2,
          SPELL_ASPECT_OF_THE_WILD_RANK_1 });
}

bool BossShadowResistanceAction::isUseful()
{
    BossShadowResistanceTrigger bossShadowResistanceTrigger(botAI, bossName);
    return bossShadowResistanceTrigger.IsActive();
}

bool BossShadowResistanceAction::Execute(Event event)
{
    return CastBestKnownSpell(botAI, bot,
        { SPELL_SHADOW_RESISTANCE_AURA_RANK_5, SPELL_SHADOW_RESISTANCE_AURA_RANK_4, SPELL_SHADOW_RESISTANCE_AURA_RANK_3,
          SPELL_SHADOW_RESISTANCE_AURA_RANK_2, SPELL_SHADOW_RESISTANCE_AURA_RANK_1 });
}
