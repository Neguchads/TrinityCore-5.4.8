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
#include "BossAuraTriggers.h"

#include "Group.h"
#include "Player.h"
#include "Playerbots.h"
#include "Unit.h"

bool BossFireResistanceTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", bossName);
    if (!boss || !boss->IsAlive() || boss->IsFriendlyTo(bot))
        return false;

    // Check if bot is paladin
    if (bot->GetClass() != CLASS_PALADIN)
        return false;

    // Check if bot have fire resistance aura
    if (bot->HasAura(SPELL_FIRE_RESISTANCE_AURA_RANK_5) || bot->HasAura(SPELL_FIRE_RESISTANCE_AURA_RANK_4) ||
        bot->HasAura(SPELL_FIRE_RESISTANCE_AURA_RANK_3) || bot->HasAura(SPELL_FIRE_RESISTANCE_AURA_RANK_2) ||
        bot->HasAura(SPELL_FIRE_RESISTANCE_AURA_RANK_1))
        return false;

    // Check that the bot actually knows the spell
    if (!bot->HasActiveSpell(SPELL_FIRE_RESISTANCE_AURA_RANK_5) &&
        !bot->HasActiveSpell(SPELL_FIRE_RESISTANCE_AURA_RANK_4) &&
        !bot->HasActiveSpell(SPELL_FIRE_RESISTANCE_AURA_RANK_3) &&
        !bot->HasActiveSpell(SPELL_FIRE_RESISTANCE_AURA_RANK_2) &&
        !bot->HasActiveSpell(SPELL_FIRE_RESISTANCE_AURA_RANK_1))
        return false;

    // Get the group and ensure it's a raid group
    Group* group = bot->GetGroup();
    if (!group || !group->isRaidGroup())
        return false;

    // Iterate through group members to find the first alive paladin
    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member || !member->IsAlive())
            continue;

        // Check if the member is a paladin
        if (member->GetClass() == CLASS_PALADIN)
        {
            // Return true only if the current bot is the first alive paladin
            return member == bot;
        }
    }

    return false;
}

bool BossFrostResistanceTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", bossName);
    if (!boss || !boss->IsAlive() || boss->IsFriendlyTo(bot))
        return false;

    // Check if bot is paladin
    if (bot->GetClass() != CLASS_PALADIN)
        return false;

    // Check if bot have frost resistance aura
    if (bot->HasAura(SPELL_FROST_RESISTANCE_AURA_RANK_5) || bot->HasAura(SPELL_FROST_RESISTANCE_AURA_RANK_4) ||
        bot->HasAura(SPELL_FROST_RESISTANCE_AURA_RANK_3) || bot->HasAura(SPELL_FROST_RESISTANCE_AURA_RANK_2) ||
        bot->HasAura(SPELL_FROST_RESISTANCE_AURA_RANK_1))
        return false;

    // Check that the bot actually knows the spell
    if (!bot->HasActiveSpell(SPELL_FROST_RESISTANCE_AURA_RANK_5) &&
        !bot->HasActiveSpell(SPELL_FROST_RESISTANCE_AURA_RANK_4) &&
        !bot->HasActiveSpell(SPELL_FROST_RESISTANCE_AURA_RANK_3) &&
        !bot->HasActiveSpell(SPELL_FROST_RESISTANCE_AURA_RANK_2) &&
        !bot->HasActiveSpell(SPELL_FROST_RESISTANCE_AURA_RANK_1))
        return false;

    // Get the group and ensure it's a raid group
    Group* group = bot->GetGroup();
    if (!group || !group->isRaidGroup())
        return false;

    // Iterate through group members to find the first alive paladin
    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member || !member->IsAlive())
            continue;

        // Check if the member is a paladin
        if (member->GetClass() == CLASS_PALADIN)
        {
            // Return true only if the current bot is the first alive paladin
            return member == bot;
        }
    }

    return false;
}

bool BossNatureResistanceTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", bossName);
    if (!boss || !boss->IsAlive() || boss->IsFriendlyTo(bot))
        return false;

    // Check if bot is alive
    if (!bot->IsAlive())
        return false;

    // Check if bot is hunter
    if (bot->GetClass() != CLASS_HUNTER)
        return false;

    // Check if bot have nature resistance aura
    if (bot->HasAura(SPELL_ASPECT_OF_THE_WILD_RANK_4) || bot->HasAura(SPELL_ASPECT_OF_THE_WILD_RANK_3) ||
        bot->HasAura(SPELL_ASPECT_OF_THE_WILD_RANK_2) || bot->HasAura(SPELL_ASPECT_OF_THE_WILD_RANK_1))
        return false;

    // Check that the bot actually knows Aspect of the Wild
    if (!bot->HasActiveSpell(SPELL_ASPECT_OF_THE_WILD_RANK_4) &&
        !bot->HasActiveSpell(SPELL_ASPECT_OF_THE_WILD_RANK_3) &&
        !bot->HasActiveSpell(SPELL_ASPECT_OF_THE_WILD_RANK_2) &&
        !bot->HasActiveSpell(SPELL_ASPECT_OF_THE_WILD_RANK_1))
        return false;

    // Get the group and ensure it's a raid group
    Group* group = bot->GetGroup();
    if (!group || !group->isRaidGroup())
        return false;

    // Iterate through group members to find the first alive hunter
    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member || !member->IsAlive())
            continue;

        // Check if the member is a hunter
        if (member->GetClass() == CLASS_HUNTER)
        {
            // Return true only if the current bot is the first alive hunter
            return member == bot;
        }
    }

    return false;
}

bool BossShadowResistanceTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", bossName);
    if (!boss || !boss->IsAlive() || boss->IsFriendlyTo(bot))
        return false;

    // Check if bot is paladin
    if (bot->GetClass() != CLASS_PALADIN)
        return false;

    // Check if bot have shadow resistance aura
    if (bot->HasAura(SPELL_SHADOW_RESISTANCE_AURA_RANK_5) ||
        bot->HasAura(SPELL_SHADOW_RESISTANCE_AURA_RANK_4) ||
        bot->HasAura(SPELL_SHADOW_RESISTANCE_AURA_RANK_3) ||
        bot->HasAura(SPELL_SHADOW_RESISTANCE_AURA_RANK_2) ||
        bot->HasAura(SPELL_SHADOW_RESISTANCE_AURA_RANK_1))
        return false;

    // Check that the bot actually knows the spell
    if (!bot->HasActiveSpell(SPELL_SHADOW_RESISTANCE_AURA_RANK_5) &&
        !bot->HasActiveSpell(SPELL_SHADOW_RESISTANCE_AURA_RANK_4) &&
        !bot->HasActiveSpell(SPELL_SHADOW_RESISTANCE_AURA_RANK_3) &&
        !bot->HasActiveSpell(SPELL_SHADOW_RESISTANCE_AURA_RANK_2) &&
        !bot->HasActiveSpell(SPELL_SHADOW_RESISTANCE_AURA_RANK_1))
        return false;

    // Get the group and ensure it's a raid group
    Group* group = bot->GetGroup();
    if (!group || !group->isRaidGroup())
        return false;

    // Iterate through group members to find the first alive paladin
    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member || !member->IsAlive())
            continue;

        // Check if the member is a paladin
        if (member->GetClass() == CLASS_PALADIN)
        {
            // Return true only if the current bot is the first alive paladin
            return member == bot;
        }
    }

    return false;
}
