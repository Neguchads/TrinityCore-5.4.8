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

#include "GenericMonkStrategy.h"

#include "Playerbots.h"

class GenericMonkStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericMonkStrategyActionNodeFactory()
    {
        creators["jab"] = &jab;
        creators["tiger palm"] = &tiger_palm;
        creators["blackout kick"] = &blackout_kick;
        creators["spear hand strike"] = &spear_hand_strike;
        creators["expel harm"] = &expel_harm;
        creators["guard"] = &guard;
        creators["fortifying brew"] = &fortifying_brew;
        creators["energizing brew"] = &energizing_brew;
        creators["tiger eye brew"] = &tiger_eye_brew;
    }

private:
    ACTION_NODE(jab, "jab");
    ACTION_NODE(tiger_palm, "tiger palm");
    ACTION_NODE(blackout_kick, "blackout kick");
    ACTION_NODE(spear_hand_strike, "spear hand strike");
    ACTION_NODE(expel_harm, "expel harm");
    ACTION_NODE(guard, "guard");
    ACTION_NODE(fortifying_brew, "fortifying brew");
    ACTION_NODE(energizing_brew, "energizing brew");
    ACTION_NODE(tiger_eye_brew, "tiger eye brew");
};

GenericMonkStrategy::GenericMonkStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI)
{
    actionNodeFactories.Add(new GenericMonkStrategyActionNodeFactory());
}

void GenericMonkStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("spear hand strike",
        NextAction::array(0, new NextAction("spear hand strike", ACTION_INTERRUPT), nullptr)));

    triggers.push_back(new TriggerNode("medium health",
        NextAction::array(0, new NextAction("expel harm", ACTION_EMERGENCY + 1), nullptr)));

    triggers.push_back(new TriggerNode("low health",
        NextAction::array(0, new NextAction("guard", ACTION_EMERGENCY), nullptr)));

    triggers.push_back(new TriggerNode("critical health",
        NextAction::array(0, new NextAction("fortifying brew", ACTION_EMERGENCY + 2), nullptr)));
}

class MonkAoeStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    MonkAoeStrategyActionNodeFactory()
    {
        creators["spinning crane kick"] = &spinning_crane_kick;
        creators["rushing jade wind"] = &rushing_jade_wind;
        creators["breath of fire"] = &breath_of_fire;
        creators["keg smash"] = &keg_smash;
    }

private:
    ACTION_NODE(spinning_crane_kick, "spinning crane kick");
    ACTION_NODE(rushing_jade_wind, "rushing jade wind");
    ACTION_NODE(breath_of_fire, "breath of fire");
    ACTION_NODE(keg_smash, "keg smash");
};

MonkAoeStrategy::MonkAoeStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI)
{
    actionNodeFactories.Add(new MonkAoeStrategyActionNodeFactory());
}

void MonkAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("medium aoe",
        NextAction::array(0,
            new NextAction("keg smash", ACTION_HIGH + 6),
            new NextAction("breath of fire", ACTION_HIGH + 5),
            new NextAction("rushing jade wind", ACTION_HIGH + 4),
            new NextAction("spinning crane kick", ACTION_HIGH + 3),
            nullptr)));

    triggers.push_back(new TriggerNode("light aoe",
        NextAction::array(0,
            new NextAction("rushing jade wind", ACTION_HIGH + 2),
            new NextAction("spinning crane kick", ACTION_HIGH + 1),
            nullptr)));
}

void MonkCureStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("detox",
        NextAction::array(0, new NextAction("detox", ACTION_DISPEL + 2), nullptr)));
    triggers.push_back(new TriggerNode("party member detox",
        NextAction::array(0, new NextAction("detox on party", ACTION_DISPEL + 1), nullptr)));
}

void MonkCcStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("paralysis",
        NextAction::array(0, new NextAction("paralysis on cc", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(new TriggerNode("clash",
        NextAction::array(0, new NextAction("clash on cc", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode("fists of fury",
        NextAction::array(0, new NextAction("fists of fury on cc", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode("dizzying haze",
        NextAction::array(0, new NextAction("dizzying haze on cc", ACTION_HIGH + 1), nullptr)));
}
