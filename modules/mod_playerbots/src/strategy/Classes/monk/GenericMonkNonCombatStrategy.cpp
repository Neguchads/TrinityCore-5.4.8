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

#include "GenericMonkNonCombatStrategy.h"

#include "Playerbots.h"

class GenericMonkNonCombatStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericMonkNonCombatStrategyActionNodeFactory()
    {
        creators["legacy of the emperor"] = &legacy_of_the_emperor;
        creators["summon jade serpent statue"] = &summon_jade_serpent_statue;
    }

private:
    ACTION_NODE(legacy_of_the_emperor, "legacy of the emperor");
    ACTION_NODE(summon_jade_serpent_statue, "summon jade serpent statue");
};

GenericMonkNonCombatStrategy::GenericMonkNonCombatStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI)
{
    actionNodeFactories.Add(new GenericMonkNonCombatStrategyActionNodeFactory());
}

void GenericMonkNonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    NonCombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("legacy of the emperor",
        NextAction::array(0, new NextAction("legacy of the emperor", ACTION_HIGH + 5), nullptr)));

    triggers.push_back(new TriggerNode("party member dead",
        NextAction::array(0, new NextAction("resuscitate", ACTION_CRITICAL_HEAL + 10), nullptr)));

    triggers.push_back(new TriggerNode("often",
        NextAction::array(0, new NextAction("summon jade serpent statue", ACTION_NORMAL + 1), nullptr)));
}
