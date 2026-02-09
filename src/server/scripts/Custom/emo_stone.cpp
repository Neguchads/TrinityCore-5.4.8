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

#include "ScriptPCH.h"

class custom_emo_stone : public ItemScript
{
public:
    custom_emo_stone() : ItemScript("custom_emo_stone") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override // Any hook here
    {
        player->PlayerTalkClass->ClearMenus(); // Clears old options
        player->ADD_GOSSIP_ITEM(0, "Summon Teleporter for |cffFF00001|r minute!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, "Close", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
        return false; // Cast the spell on use normally
    }

    void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
				if (!player->FindNearestCreature(128, 100.0f, true))
				{
					player->CLOSE_GOSSIP_MENU();
					player->SummonCreature(128, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), -1.52f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
				}
				else
					player->CLOSE_GOSSIP_MENU();
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->CLOSE_GOSSIP_MENU();
                break;
        }
        player->CLOSE_GOSSIP_MENU();
    }
};

void AddSC_custom_emo_stone() // Add to scriptloader normally
{
    new custom_emo_stone();
}
