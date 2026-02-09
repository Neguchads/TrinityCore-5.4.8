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

#include "BattlegroundStrategy.h"

#include "Playerbots.h"

void BGStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("bg join", relevance), nullptr)));
    triggers.push_back(new TriggerNode("bg invite active",
        NextAction::array(0, new NextAction("bg status check", relevance), nullptr)));
    triggers.push_back(new TriggerNode("timer",
        NextAction::array(0, new NextAction("bg strategy check", relevance), nullptr)));
}

BGStrategy::BGStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

void BattlegroundStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("bg waiting", NextAction::array(0, new NextAction("bg move to start", ACTION_BG), nullptr)));
    triggers.push_back(new TriggerNode("bg active", NextAction::array(0, new NextAction("bg move to objective", ACTION_BG), nullptr)));
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("bg check objective", ACTION_BG + 1), nullptr)));
    triggers.push_back(new TriggerNode("dead", NextAction::array(0, new NextAction("bg reset objective force", ACTION_EMERGENCY), nullptr)));
}

BattlegroundStrategy::BattlegroundStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

void WarsongStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("bg active", NextAction::array(0, new NextAction("bg check flag", ACTION_EMERGENCY), nullptr)));
    triggers.push_back(new TriggerNode("enemy flagcarrier near", NextAction::array(0, new NextAction("attack enemy flag carrier", ACTION_RAID + 1.0f), nullptr)));
    triggers.push_back(new TriggerNode("team flagcarrier near", NextAction::array(0, new NextAction("bg protect fc", ACTION_RAID), nullptr)));
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("bg use buff", ACTION_BG), nullptr)));
    triggers.push_back(new TriggerNode("low health", NextAction::array(0, new NextAction("bg use buff", ACTION_MOVE), nullptr)));
    triggers.push_back(new TriggerNode("low mana", NextAction::array(0, new NextAction("bg use buff", ACTION_MOVE), nullptr)));
    triggers.push_back(new TriggerNode("player has flag", NextAction::array(0, new NextAction("bg move to objective", ACTION_EMERGENCY), nullptr)));
    triggers.push_back(new TriggerNode("timer bg", NextAction::array(0, new NextAction("bg reset objective force", ACTION_EMERGENCY), nullptr)));
}

void AlteracStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("alliance no snowfall gy", NextAction::array(0, new NextAction("bg move to objective", ACTION_EMERGENCY), nullptr)));
    triggers.push_back(new TriggerNode("timer bg", NextAction::array(0, new NextAction("bg reset objective force", ACTION_EMERGENCY), nullptr)));
}

void ArathiStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("bg active", NextAction::array(0, new NextAction("bg check flag", ACTION_EMERGENCY), nullptr)));
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("bg use buff", ACTION_BG), nullptr)));
    triggers.push_back(new TriggerNode("low health", NextAction::array(0, new NextAction("bg use buff", ACTION_MOVE), nullptr)));
    triggers.push_back(new TriggerNode("low mana", NextAction::array(0, new NextAction("bg use buff", ACTION_MOVE), nullptr)));
}

void EyeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("bg active", NextAction::array(0, new NextAction("bg check flag", ACTION_EMERGENCY), nullptr)));
    triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction("bg use buff", ACTION_BG), nullptr)));
    triggers.push_back(new TriggerNode("low health", NextAction::array(0, new NextAction("bg use buff", ACTION_MOVE), nullptr)));
    triggers.push_back(new TriggerNode("low mana", NextAction::array(0, new NextAction("bg use buff", ACTION_MOVE), nullptr)));
    triggers.push_back(new TriggerNode("enemy flagcarrier near", NextAction::array(0, new NextAction("attack enemy flag carrier", ACTION_RAID), nullptr)));
    triggers.push_back(new TriggerNode("player has flag", NextAction::array(0, new NextAction("bg move to objective", ACTION_EMERGENCY), nullptr)));
}

void IsleStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("bg active", NextAction::array(0, new NextAction("bg check flag", ACTION_MOVE), nullptr)));
    triggers.push_back(new TriggerNode("timer", NextAction::array(0, new NextAction("enter vehicle", ACTION_MOVE + 8.0f), nullptr)));
    triggers.push_back(new TriggerNode("random", NextAction::array(0, new NextAction("leave vehicle", ACTION_MOVE + 7.0f), nullptr)));
    triggers.push_back(new TriggerNode("in vehicle", NextAction::array(0, new NextAction("hurl boulder", ACTION_MOVE + 9.0f), nullptr)));
    triggers.push_back(new TriggerNode("in vehicle", NextAction::array(0, new NextAction("fire cannon", ACTION_MOVE + 9.0f), nullptr)));
    triggers.push_back(new TriggerNode("in vehicle", NextAction::array(0, new NextAction("napalm", ACTION_MOVE + 9.0f), nullptr)));
    triggers.push_back(new TriggerNode("enemy is close", NextAction::array(0, new NextAction("steam blast", ACTION_MOVE + 9.0f), nullptr)));
    triggers.push_back(new TriggerNode("in vehicle", NextAction::array(0, new NextAction("ram", ACTION_MOVE + 9.0f), nullptr)));
    triggers.push_back(new TriggerNode("enemy is close", NextAction::array(0, new NextAction("ram", ACTION_MOVE + 9.1f), nullptr)));
    triggers.push_back(new TriggerNode("enemy out of melee", NextAction::array(0, new NextAction("steam rush", ACTION_MOVE + 9.2f), nullptr)));
    triggers.push_back(new TriggerNode("in vehicle", NextAction::array(0, new NextAction("incendiary rocket", ACTION_MOVE + 9.0f), nullptr)));
    triggers.push_back(new TriggerNode("in vehicle", NextAction::array(0, new NextAction("rocket blast", ACTION_MOVE + 9.0f), nullptr)));
    triggers.push_back(new TriggerNode("in vehicle", NextAction::array(0, new NextAction("glaive throw", ACTION_MOVE + 9.0f), nullptr)));
}

void ArenaStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("no possible targets", NextAction::array(0, new NextAction("arena tactics", ACTION_BG), nullptr)));
}
