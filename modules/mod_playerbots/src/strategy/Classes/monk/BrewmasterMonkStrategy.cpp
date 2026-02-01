/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "BrewmasterMonkStrategy.h"

#include "Playerbots.h"

class BrewmasterMonkStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    BrewmasterMonkStrategyActionNodeFactory()
    {
        creators["keg smash"] = &keg_smash;
        creators["breath of fire"] = &breath_of_fire;
        creators["purifying brew"] = &purifying_brew;
        creators["elusive brew"] = &elusive_brew;
        creators["guard"] = &guard;
        creators["dizzying haze"] = &dizzying_haze;
        creators["clash"] = &clash;
        creators["summon black ox statue"] = &summon_black_ox_statue;
    }

private:
    ACTION_NODE(keg_smash, "keg smash");
    ACTION_NODE(breath_of_fire, "breath of fire");
    ACTION_NODE(purifying_brew, "purifying brew");
    ACTION_NODE(elusive_brew, "elusive brew");
    ACTION_NODE(guard, "guard");
    ACTION_NODE(dizzying_haze, "dizzying haze");
    ACTION_NODE(clash, "clash");
    ACTION_NODE(summon_black_ox_statue, "summon black ox statue");
};

BrewmasterMonkStrategy::BrewmasterMonkStrategy(PlayerbotAI* botAI) : GenericMonkStrategy(botAI)
{
    actionNodeFactories.Add(new BrewmasterMonkStrategyActionNodeFactory());
}

NextAction** BrewmasterMonkStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("summon black ox statue", ACTION_DEFAULT + 0.9f),
        new NextAction("dizzying haze", ACTION_DEFAULT + 0.8f),
        new NextAction("keg smash", ACTION_DEFAULT + 0.7f),
        new NextAction("blackout kick", ACTION_DEFAULT + 0.6f),
        new NextAction("breath of fire", ACTION_DEFAULT + 0.5f),
        new NextAction("chi wave", ACTION_DEFAULT + 0.45f),
        new NextAction("tiger palm", ACTION_DEFAULT + 0.4f),
        new NextAction("jab", ACTION_DEFAULT + 0.3f),
        new NextAction("melee", ACTION_DEFAULT),
        nullptr);
}

void BrewmasterMonkStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMonkStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("stance of the sturdy ox",
        NextAction::array(0, new NextAction("stance of the sturdy ox", ACTION_HIGH + 9), nullptr)));

    triggers.push_back(new TriggerNode("tiger power",
        NextAction::array(0, new NextAction("tiger palm", ACTION_HIGH + 7), nullptr)));

    triggers.push_back(new TriggerNode("shuffle",
        NextAction::array(0, new NextAction("blackout kick", ACTION_HIGH + 6), nullptr)));

    triggers.push_back(new TriggerNode("keg smash",
        NextAction::array(0, new NextAction("keg smash", ACTION_HIGH + 5), nullptr)));

    triggers.push_back(new TriggerNode("heavy stagger",
        NextAction::array(0, new NextAction("purifying brew", ACTION_EMERGENCY + 1), nullptr)));

    triggers.push_back(new TriggerNode("moderate stagger",
        NextAction::array(0, new NextAction("purifying brew", ACTION_HIGH + 2), nullptr)));

    triggers.push_back(new TriggerNode("medium health",
        NextAction::array(0, new NextAction("guard", ACTION_HIGH + 3), nullptr)));

    triggers.push_back(new TriggerNode("often",
        NextAction::array(0,
            new NextAction("summon black ox statue", ACTION_NORMAL + 2),
            new NextAction("chi brew", ACTION_NORMAL + 1),
            nullptr)));

    triggers.push_back(new TriggerNode("light aoe",
        NextAction::array(0,
            new NextAction("breath of fire", ACTION_HIGH + 2),
            new NextAction("spinning crane kick", ACTION_HIGH + 1),
            nullptr)));
}
