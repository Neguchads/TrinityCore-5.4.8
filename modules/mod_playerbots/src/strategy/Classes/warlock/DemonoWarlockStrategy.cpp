#include "DemonoWarlockStrategy.h"
#include "Playerbots.h"

class DemonoWarlockStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DemonoWarlockStrategyActionNodeFactory()
    {
        creators["dark soul: knowledge"] = &dark_soul;
        creators["hellfire"] = &hellfire;
        creators["hand of guldan"] = &hand_of_guldan;
        creators["soul fire"] = &soul_fire;

        creators["metamorphosis"] = &metamorphosis;
        creators["chaos wave"] = &metamorphosis_chaos_wave;
        creators["doom"] = &metamorphosis_doom;
        creators["immolation aura"] = &metamorphosis_immolation;
        creators["touch of chaos"] = &metamorphosis_touch_chaos;
        creators["void ray"] = &metamorphosis_void_ray;
    }

private:
    ACTION_NODE(metamorphosis, "metamorphosis");
    ACTION_NODE_A(metamorphosis_chaos_wave, "chaos wave", "hand of guldan");
    ACTION_NODE_A(metamorphosis_doom, "doom", "corruption");
    ACTION_NODE_A(metamorphosis_immolation, "immolation aura", "hellfire");
    ACTION_NODE_A(metamorphosis_touch_chaos, "touch of chaos", "shadow bolt");
    ACTION_NODE_A(metamorphosis_void_ray, "void ray", "fel flame");

    ACTION_NODE(dark_soul, "dark soul: knowledge");
    ACTION_NODE(hellfire, "hellfire");
    ACTION_NODE(hand_of_guldan, "hand of guldan");
    ACTION_NODE(soul_fire, "soul fire");
};

DemonoWarlockStrategy::DemonoWarlockStrategy(PlayerbotAI* botAI) : GenericWarlockStrategy(botAI)
{
    actionNodeFactories.Add(new DemonoWarlockStrategyActionNodeFactory());
}

NextAction** DemonoWarlockStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("dark soul: knowledge", ACTION_DEFAULT + 0.8f),
        new NextAction("hand of guldan", ACTION_DEFAULT + 0.7f),
        new NextAction("soul fire", ACTION_DEFAULT + 0.6f),
        new NextAction("doom", ACTION_DEFAULT + 0.5f),
        new NextAction("touch of chaos", ACTION_DEFAULT + 0.4f),
        new NextAction("shadow bolt", ACTION_DEFAULT + 0.3f),
        new NextAction("fel flame", ACTION_DEFAULT + 0.2f),
        nullptr);
}

void DemonoWarlockStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarlockStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("corruption", NextAction::array(0, new NextAction("corruption", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("metamorphosis",
                                       NextAction::array(0, new NextAction("metamorphosis", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode("molten core",
                                       NextAction::array(0, new NextAction("soul fire", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode("hand of guldan",
                                       NextAction::array(0, new NextAction("hand of guldan", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(new TriggerNode("soul fire",
                                       NextAction::array(0, new NextAction("soul fire", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode("dark soul: knowledge",
                                       NextAction::array(0, new NextAction("dark soul: knowledge", ACTION_HIGH + 1),
                                                         nullptr)));
}

