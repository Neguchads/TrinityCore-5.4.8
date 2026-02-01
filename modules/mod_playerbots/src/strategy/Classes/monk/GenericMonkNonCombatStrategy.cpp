/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
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
