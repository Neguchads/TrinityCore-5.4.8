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
#ifndef _PLAYERBOT_OPERATIONS_H
#define _PLAYERBOT_OPERATIONS_H

#include "PlayerbotOperation.h"

#include "ObjectAccessor.h"
#include "Player.h"
#include "Playerbots.h"

/**
 * @brief Bot logout group cleanup operation
 *
 * Runs in world thread to safely touch group state and bot AI.
 */
class BotLogoutGroupCleanupOperation : public PlayerbotOperation
{
public:
    explicit BotLogoutGroupCleanupOperation(ObjectGuid botGuid) : m_botGuid(botGuid) {}

    bool Execute() override
    {
        Player* bot = ObjectAccessor::FindPlayer(m_botGuid);
        if (!bot)
            return false;

        PlayerbotAI* botAI = sPlayerbotsMgr->GetPlayerbotAI(bot);
        if (!botAI)
            return false;

        Group* group = bot->GetGroup();
        if (group && !bot->InBattleground() && !bot->InBattlegroundQueue() && botAI->HasActivePlayerMaster())
        {
            // Placeholder for future group/AI persistence or cleanup.
        }

        return true;
    }

    ObjectGuid GetBotGuid() const override { return m_botGuid; }
    uint32 GetPriority() const override { return 70; }
    std::string GetName() const override { return "BotLogoutGroupCleanup"; }

    bool IsValid() const override
    {
        return ObjectAccessor::FindPlayer(m_botGuid) != nullptr;
    }

private:
    ObjectGuid m_botGuid;
};

#endif
