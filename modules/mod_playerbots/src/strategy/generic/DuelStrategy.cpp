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

#include "DuelStrategy.h"

#include "Playerbots.h"

void DuelStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    PassTroughStrategy::InitTriggers(triggers);

    triggers.push_back(
        new TriggerNode("duel requested", NextAction::array(0, new NextAction("accept duel", relevance), nullptr)));
    triggers.push_back(
        new TriggerNode("no attackers", NextAction::array(0, new NextAction("attack duel opponent", 70.0f), nullptr)));
}

DuelStrategy::DuelStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

void StartDuelStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("often", NextAction::array(0, new NextAction("start duel", ACTION_DEFAULT), nullptr)));
}

StartDuelStrategy::StartDuelStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}
