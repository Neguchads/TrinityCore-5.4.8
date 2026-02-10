#include "DestruWarlockStrategy.h"
#include "Playerbots.h"

class DestruWarlockStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DestruWarlockStrategyActionNodeFactory()
    {
        creators["dark soul: instability"] = &dark_soul;
        creators["chaos bolt"] = &chaos_bolt;
        creators["conflagrate"] = &conflagrate;
        creators["fire and brimstone"] = &fire_and_brimstone;

        creators["flames of xoroth"] = &flames_of_xoroth;
        creators["immolate"] = &immolate;
        creators["incinerate"] = &incinerate;
        creators["shadowburn"] = &shadowburn;
    }

private:
    ACTION_NODE(dark_soul, "dark soul: instability");
    ACTION_NODE(chaos_bolt, "chaos bolt");
    ACTION_NODE(conflagrate, "conflagrate");
    ACTION_NODE(fire_and_brimstone, "fire and brimstone");
    ACTION_NODE(flames_of_xoroth, "flames of xoroth");
    ACTION_NODE_C(immolate, "immolate", "conflagrate");
    ACTION_NODE(incinerate, "incinerate");
    ACTION_NODE(shadowburn, "shadowburn");
};

DestruWarlockStrategy::DestruWarlockStrategy(PlayerbotAI* botAI) : GenericWarlockStrategy(botAI)
{
    actionNodeFactories.Add(new DestruWarlockStrategyActionNodeFactory());
}

NextAction** DestruWarlockStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("immolate", ACTION_DEFAULT + 0.7f),
        new NextAction("conflagrate", ACTION_DEFAULT + 0.6f),
        new NextAction("chaos bolt", ACTION_DEFAULT + 0.5f),
        new NextAction("shadowburn", ACTION_DEFAULT + 0.4f),
        new NextAction("incinerate", ACTION_DEFAULT + 0.3f),
        new NextAction("fel flame", ACTION_DEFAULT + 0.2f),
        nullptr);
}

void DestruWarlockStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarlockStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("immolate", NextAction::array(0, new NextAction("immolate", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("backdraft", NextAction::array(0, new NextAction("incinerate", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("conflagrate", NextAction::array(0, new NextAction("conflagrate", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode("fire and brimstone", NextAction::array(0, new NextAction("fire and brimstone", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("target critical health", NextAction::array(0, new NextAction("shadowburn", ACTION_HIGH + 20), nullptr)));
    triggers.push_back(new TriggerNode("dark soul: instability",
                                       NextAction::array(0, new NextAction("dark soul: instability", ACTION_HIGH + 1),
                                                         nullptr)));
    triggers.push_back(new TriggerNode("medium aoe", NextAction::array(0, new NextAction("rain of fire", 21.0f), nullptr)));
    triggers.push_back(new TriggerNode("high aoe", NextAction::array(0, new NextAction("fire and brimstone", 22.0f), nullptr)));
}

