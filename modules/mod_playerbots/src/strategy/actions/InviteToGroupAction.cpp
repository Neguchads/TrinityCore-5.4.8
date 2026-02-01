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

#include "InviteToGroupAction.h"

#include <sstream>
#include <unordered_map>

#include "Event.h"
#include "Group.h"
#include "LFGMgr.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotSpec.h"
#include "Playerbots.h"
#include "WorldPacket.h"

bool InviteToGroupAction::Invite(Player* inviter, Player* player)
{
    if (!player || !inviter)
        return false;

    if (inviter == player)
        return false;

    Group* group = inviter->GetGroup();
    if (group && group->isBGGroup())
        group = inviter->GetOriginalGroup();

    Group* targetGroup = player->GetGroup();
    if (targetGroup && targetGroup->isBGGroup())
        targetGroup = player->GetOriginalGroup();

    if (targetGroup || player->GetGroupInvite())
        return false;

    if (group)
    {
        if (!group->isRaidGroup() && group->GetMembersCount() > 4 && group->IsLeader(inviter->GetGUID()))
            group->ConvertToRaid();

        if (group->IsFull())
            return false;

        if (!group->AddInvite(player))
            return false;
    }
    else
    {
        group = new Group();
        if (!group->AddLeaderInvite(inviter) || !group->AddInvite(player))
        {
            delete group;
            return false;
        }
    }

    player->GetSession()->SendGroupInviteNotification(inviter->GetName(), false);
    return true;
}

static BotRoles GetRoleForPlayer(Player* player)
{
    if (!player)
        return BOT_ROLE_NONE;

    if (PlayerBotSpec::IsTank(player, true))
        return BOT_ROLE_TANK;
    if (PlayerBotSpec::IsHeal(player, true))
        return BOT_ROLE_HEALER;
    return BOT_ROLE_DPS;
}

bool LfgAction::Execute(Event event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    if (!requester)
        return false;

    if (bot->InBattleground() || bot->InBattlegroundQueue())
        return false;

    if (!botAI->IsSafe(requester))
        return false;

    if (requester->GetLevel() > bot->GetLevel() + 4 || bot->GetLevel() > requester->GetLevel() + 4)
        return false;

    std::string param = event.getParam();
    if (!param.empty() && param != "40" && param != "25" && param != "20" && param != "10" && param != "5")
        return false;

    Group* group = requester->GetGroup();

    std::unordered_map<BotRoles, uint32> allowedRoles;
    allowedRoles[BOT_ROLE_TANK] = 1;
    allowedRoles[BOT_ROLE_HEALER] = 1;
    allowedRoles[BOT_ROLE_DPS] = 3;

    if (group)
    {
        if (param.empty() && group->isRaidGroup())
            param = "25";

        if (param == "40")
        {
            allowedRoles[BOT_ROLE_TANK] = 4;
            allowedRoles[BOT_ROLE_HEALER] = 16;
            allowedRoles[BOT_ROLE_DPS] = 20;
        }
        else if (param == "25")
        {
            allowedRoles[BOT_ROLE_TANK] = 3;
            allowedRoles[BOT_ROLE_HEALER] = 7;
            allowedRoles[BOT_ROLE_DPS] = 15;
        }
        else if (param == "20")
        {
            allowedRoles[BOT_ROLE_TANK] = 2;
            allowedRoles[BOT_ROLE_HEALER] = 5;
            allowedRoles[BOT_ROLE_DPS] = 13;
        }
        else if (param == "10")
        {
            allowedRoles[BOT_ROLE_TANK] = 2;
            allowedRoles[BOT_ROLE_HEALER] = 3;
            allowedRoles[BOT_ROLE_DPS] = 5;
        }

        if (group->IsFull())
        {
            if (param.empty() || param == "5" || group->isRaidGroup())
                return false;

            if (group->IsLeader(requester->GetGUID()))
                group->ConvertToRaid();
        }

        Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); ++itr)
        {
            Player* player = ObjectAccessor::FindPlayer(itr->guid);
            if (!botAI->IsSafe(player))
                return false;

            BotRoles role = GetRoleForPlayer(player);
            if (allowedRoles[role] > 0)
                allowedRoles[role]--;
        }
    }
    else
    {
        BotRoles role = GetRoleForPlayer(requester);
        if (allowedRoles[role] > 0)
            allowedRoles[role]--;
    }

    BotRoles botRole = GetRoleForPlayer(bot);
    if (allowedRoles[botRole] == 0)
        return false;

    if (bot->GetGroup())
    {
        if (botAI->HasRealPlayerMaster())
            return false;

        if (!botAI->DoSpecificAction("leave", event, true))
            return false;
    }

    if (!Invite(requester, bot))
        return false;

    Event acceptEvent("accept invitation", requester->GetGUID());
    if (!botAI->DoSpecificAction("accept invitation", acceptEvent, true))
        return false;

    return true;
}
