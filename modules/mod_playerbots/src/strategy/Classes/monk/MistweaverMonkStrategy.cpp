/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "MistweaverMonkStrategy.h"

#include "Playerbots.h"

class MistweaverMonkStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    MistweaverMonkStrategyActionNodeFactory()
    {
        creators["soothing mist on party"] = &soothing_mist_on_party;
        creators["surging mist on party"] = &surging_mist_on_party;
        creators["enveloping mist on party"] = &enveloping_mist_on_party;
        creators["renewing mist on party"] = &renewing_mist_on_party;
        creators["uplift"] = &uplift;
        creators["revival"] = &revival;
        creators["life cocoon on party"] = &life_cocoon_on_party;
        creators["mana tea"] = &mana_tea;
    }

private:
    ACTION_NODE(soothing_mist_on_party, "soothing mist on party");
    ACTION_NODE(surging_mist_on_party, "surging mist on party");
    ACTION_NODE(enveloping_mist_on_party, "enveloping mist on party");
    ACTION_NODE(renewing_mist_on_party, "renewing mist on party");
    ACTION_NODE(uplift, "uplift");
    ACTION_NODE(revival, "revival");
    ACTION_NODE(life_cocoon_on_party, "life cocoon on party");
    ACTION_NODE(mana_tea, "mana tea");
};

MistweaverMonkStrategy::MistweaverMonkStrategy(PlayerbotAI* botAI) : GenericMonkStrategy(botAI)
{
    actionNodeFactories.Add(new MistweaverMonkStrategyActionNodeFactory());
}

void MistweaverMonkStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMonkStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("stance of the wise serpent",
        NextAction::array(0, new NextAction("stance of the wise serpent", ACTION_HIGH + 9), nullptr)));

    triggers.push_back(new TriggerNode("often",
        NextAction::array(0, new NextAction("summon jade serpent statue", ACTION_NORMAL + 1), nullptr)));

    triggers.push_back(new TriggerNode("group heal setting",
        NextAction::array(0,
            new NextAction("thunder focus tea", ACTION_HIGH + 8),
            new NextAction("uplift", ACTION_HIGH + 7),
            new NextAction("revival", ACTION_HIGH + 6),
            nullptr)));

    triggers.push_back(new TriggerNode("party member critical health",
        NextAction::array(0,
            new NextAction("life cocoon on party", ACTION_EMERGENCY + 2),
            new NextAction("surging mist on party", ACTION_CRITICAL_HEAL + 1),
            new NextAction("enveloping mist on party", ACTION_CRITICAL_HEAL),
            nullptr)));

    triggers.push_back(new TriggerNode("party member low health",
        NextAction::array(0,
            new NextAction("healing sphere", ACTION_HIGH + 3),
            new NextAction("enveloping mist on party", ACTION_HIGH + 2),
            new NextAction("surging mist on party", ACTION_HIGH + 1),
            new NextAction("renewing mist on party", ACTION_HIGH),
            nullptr)));

    triggers.push_back(new TriggerNode("party member medium health",
        NextAction::array(0,
            new NextAction("healing sphere", ACTION_NORMAL + 2),
            new NextAction("renewing mist on party", ACTION_NORMAL + 2),
            new NextAction("soothing mist on party", ACTION_NORMAL + 1),
            nullptr)));

    triggers.push_back(new TriggerNode("party member almost full health",
        NextAction::array(0,
            new NextAction("renewing mist on party", ACTION_NORMAL),
            nullptr)));

    triggers.push_back(new TriggerNode("often",
        NextAction::array(0,
            new NextAction("chi wave", ACTION_NORMAL + 2),
            new NextAction("chi burst", ACTION_NORMAL + 1),
            new NextAction("zen sphere", ACTION_NORMAL),
            nullptr)));

    triggers.push_back(new TriggerNode("light aoe",
        NextAction::array(0, new NextAction("rushing jade wind", ACTION_NORMAL + 3), nullptr)));

    triggers.push_back(new TriggerNode("often",
        NextAction::array(0,
            new NextAction("chi torpedo", ACTION_NORMAL + 1),
            new NextAction("invoke xuen", ACTION_NORMAL),
            nullptr)));

    triggers.push_back(new TriggerNode("medium mana",
        NextAction::array(0, new NextAction("mana tea", ACTION_HIGH + 5), nullptr)));

    triggers.push_back(new TriggerNode("party member to heal out of spell range",
        NextAction::array(0, new NextAction("reach party member to heal", ACTION_CRITICAL_HEAL + 1), nullptr)));
}
