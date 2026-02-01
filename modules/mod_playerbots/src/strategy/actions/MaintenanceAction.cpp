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

#include "MaintenanceAction.h"

#include "BotFactory.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"

bool MaintenanceAction::Execute(Event /*event*/)
{
    if (!sPlayerbotAIConfig->maintenanceCommand)
    {
        botAI->TellError("maintenance command is not allowed, please check the configuration.");
        return false;
    }

    BotFactory factory(bot, bot->GetLevel());

    if (!botAI->IsAlt())
    {
        factory.InitBags(false);
        factory.InitFood();
        factory.InitReagents();
        factory.InitConsumables();
        factory.InitPotions();
        factory.InitTalentsTree(true);
        factory.InitPet();
        factory.InitPetTalents();
        factory.InitSkills();
        factory.InitClassSpells();
        factory.InitAvailableSpells();
        factory.InitSpecialSpells();
        factory.InitMounts();
        factory.InitGlyphs(false);
        factory.InitReputation();
        if (bot->GetLevel() >= sPlayerbotAIConfig->minEnchantingBotLevel)
            factory.ApplyEnchantAndGemsNew();
    }
    else
    {
        if (sPlayerbotAIConfig->altMaintenanceBags)
            factory.InitBags(false);
        if (sPlayerbotAIConfig->altMaintenanceFood)
            factory.InitFood();
        if (sPlayerbotAIConfig->altMaintenanceReagents)
            factory.InitReagents();
        if (sPlayerbotAIConfig->altMaintenanceConsumables)
            factory.InitConsumables();
        if (sPlayerbotAIConfig->altMaintenancePotions)
            factory.InitPotions();
        if (sPlayerbotAIConfig->altMaintenanceTalentTree)
            factory.InitTalentsTree(true);
        if (sPlayerbotAIConfig->altMaintenancePet)
            factory.InitPet();
        if (sPlayerbotAIConfig->altMaintenancePetTalents)
            factory.InitPetTalents();
        if (sPlayerbotAIConfig->altMaintenanceSkills)
            factory.InitSkills();
        if (sPlayerbotAIConfig->altMaintenanceClassSpells)
            factory.InitClassSpells();
        if (sPlayerbotAIConfig->altMaintenanceAvailableSpells)
            factory.InitAvailableSpells();
        if (sPlayerbotAIConfig->altMaintenanceSpecialSpells)
            factory.InitSpecialSpells();
        if (sPlayerbotAIConfig->altMaintenanceMounts)
            factory.InitMounts();
        if (sPlayerbotAIConfig->altMaintenanceGlyphs)
            factory.InitGlyphs(false);
        if (sPlayerbotAIConfig->altMaintenanceReputation)
            factory.InitReputation();
        if (sPlayerbotAIConfig->altMaintenanceGemsEnchants && bot->GetLevel() >= sPlayerbotAIConfig->minEnchantingBotLevel)
            factory.ApplyEnchantAndGemsNew();
    }

    bot->DurabilityRepairAll(false, 1.0f, false);
    bot->SendTalentsInfoData();

    return true;
}
