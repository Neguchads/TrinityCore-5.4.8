/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "WindwalkerMonkStrategy.h"

#include "Playerbots.h"

class WindwalkerMonkStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    WindwalkerMonkStrategyActionNodeFactory()
    {
        creators["rising sun kick"] = &rising_sun_kick;
        creators["fists of fury"] = &fists_of_fury;
        creators["tiger palm"] = &tiger_palm;
        creators["blackout kick"] = &blackout_kick;
        creators["tiger eye brew"] = &tiger_eye_brew;
        creators["energizing brew"] = &energizing_brew;
        creators["touch of death"] = &touch_of_death;
    }

private:
    ACTION_NODE(rising_sun_kick, "rising sun kick");
    ACTION_NODE(fists_of_fury, "fists of fury");
    ACTION_NODE(tiger_palm, "tiger palm");
    ACTION_NODE(blackout_kick, "blackout kick");
    ACTION_NODE(tiger_eye_brew, "tiger eye brew");
    ACTION_NODE(energizing_brew, "energizing brew");
    ACTION_NODE(touch_of_death, "touch of death");
};

WindwalkerMonkStrategy::WindwalkerMonkStrategy(PlayerbotAI* botAI) : GenericMonkStrategy(botAI)
{
    actionNodeFactories.Add(new WindwalkerMonkStrategyActionNodeFactory());
}

NextAction** WindwalkerMonkStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("rising sun kick", ACTION_DEFAULT + 0.7f),
        new NextAction("fists of fury", ACTION_DEFAULT + 0.6f),
        new NextAction("blackout kick", ACTION_DEFAULT + 0.5f),
        new NextAction("tiger palm", ACTION_DEFAULT + 0.4f),
        new NextAction("chi wave", ACTION_DEFAULT + 0.3f),
        new NextAction("tiger eye brew", ACTION_DEFAULT + 0.25f),
        new NextAction("energizing brew", ACTION_DEFAULT + 0.2f),
        new NextAction("jab", ACTION_DEFAULT + 0.2f),
        new NextAction("melee", ACTION_DEFAULT),
        nullptr);
}

void WindwalkerMonkStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMonkStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("stance of the fierce tiger",
        NextAction::array(0, new NextAction("stance of the fierce tiger", ACTION_HIGH + 9), nullptr)));

    triggers.push_back(new TriggerNode("tiger power",
        NextAction::array(0, new NextAction("tiger palm", ACTION_HIGH + 8), nullptr)));

    triggers.push_back(new TriggerNode("rising sun kick",
        NextAction::array(0, new NextAction("rising sun kick", ACTION_HIGH + 7), nullptr)));

    triggers.push_back(new TriggerNode("high energy available",
        NextAction::array(0, new NextAction("jab", ACTION_HIGH + 3), nullptr)));

    triggers.push_back(new TriggerNode("target critical health",
        NextAction::array(0, new NextAction("touch of death", ACTION_HIGH + 10), nullptr)));
}
