

#include "CasterDruidStrategy.h"

#include "AiObjectContext.h"
#include "FeralDruidStrategy.h"

class CasterDruidStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    CasterDruidStrategyActionNodeFactory()
    {
        creators["faerie fire"] = &faerie_fire;
        creators["hibernate"] = &hibernate;
        creators["entangling roots"] = &entangling_roots;
        creators["entangling roots on cc"] = &entangling_roots_on_cc;
        creators["wrath"] = &wrath;
        creators["starfall"] = &starfall;
        creators["insect swarm"] = &insect_swarm;
        creators["moonfire"] = &moonfire;
        creators["sunfire"] = &sunfire;
        creators["starfire"] = &starfire;
        creators["starsurge"] = &starsurge;
        creators["astral communion"] = &astral_communion;
        creators["celestial alignment"] = &celestial_alignment;
        creators["wild mushroom"] = &wild_mushroom;
        creators["wild mushroom: detonate"] = &wild_mushroom_detonate;
        creators["astral storm"] = &astral_storm;
        creators["moonkin form"] = &moonkin_form;
    }

private:
    static ActionNode* faerie_fire([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("faerie fire",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* hibernate([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("hibernate",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ NextAction::array(0, new NextAction("entangling roots"), nullptr),
                              /*C*/ nullptr);
    }

    static ActionNode* entangling_roots([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("entangling roots",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* entangling_roots_on_cc([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("entangling roots on cc",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* wrath([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("wrath",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* starfall([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("starfall",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* insect_swarm([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("insect swarm",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* moonfire([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("moonfire",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* sunfire([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("sunfire",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* starfire([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("starfire",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* starsurge([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("starsurge",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* astral_communion([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("astral communion",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* celestial_alignment([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("celestial alignment",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* wild_mushroom([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("wild mushroom",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* wild_mushroom_detonate([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("wild mushroom: detonate",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* astral_storm([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("astral storm",
                              /*P*/ NextAction::array(0, new NextAction("moonkin form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* moonkin_form([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("moonkin form",
                              /*P*/ NextAction::array(0, new NextAction("caster form"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
};

CasterDruidStrategy::CasterDruidStrategy(PlayerbotAI* botAI) : GenericDruidStrategy(botAI)
{
    actionNodeFactories.Add(new CasterDruidStrategyActionNodeFactory());
    actionNodeFactories.Add(new ShapeshiftDruidStrategyActionNodeFactory());
}

NextAction** CasterDruidStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("starfall", ACTION_DEFAULT + 0.7f),
                             new NextAction("force of nature", ACTION_DEFAULT + 0.6f),
                             new NextAction("moonfire", ACTION_DEFAULT + 0.5f),
                             new NextAction("insect swarm", ACTION_DEFAULT + 0.4f),
                             new NextAction("starfire", ACTION_DEFAULT + 0.3f),
                             new NextAction("wrath", ACTION_DEFAULT + 0.2f),
                             nullptr);
}

void CasterDruidStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericDruidStrategy::InitTriggers(triggers);

    // triggers.push_back(new TriggerNode("enemy out of spell", NextAction::array(0, new NextAction("reach spell",
    // ACTION_MOVE), nullptr)));
    triggers.push_back(new TriggerNode("eclipse (lunar) cooldown",
                                       NextAction::array(0, new NextAction("starfire", ACTION_DEFAULT + 0.2f), nullptr)));
    triggers.push_back(new TriggerNode("eclipse (solar) cooldown",
                                       NextAction::array(0, new NextAction("wrath", ACTION_DEFAULT + 0.2f), nullptr)));
                                       
    triggers.push_back(new TriggerNode(
        "insect swarm", NextAction::array(0, new NextAction("insect swarm", ACTION_NORMAL + 5), nullptr)));
    triggers.push_back(new TriggerNode(
        "sunfire", NextAction::array(0, new NextAction("sunfire", ACTION_NORMAL + 5), nullptr)));
    triggers.push_back(
        new TriggerNode("moonfire", NextAction::array(0, new NextAction("moonfire", ACTION_NORMAL + 4), nullptr)));
    triggers.push_back(
        new TriggerNode("eclipse (solar)", NextAction::array(0, new NextAction("wrath", ACTION_NORMAL + 6), nullptr)));
    triggers.push_back(new TriggerNode("eclipse (lunar)",
                                       NextAction::array(0, new NextAction("starfire", ACTION_NORMAL + 6), nullptr)));
    triggers.push_back(
        new TriggerNode("medium mana", NextAction::array(0, new NextAction("innervate", ACTION_HIGH + 9), NULL)));
    triggers.push_back(new TriggerNode("enemy too close for spell",
                                       NextAction::array(0, new NextAction("flee", ACTION_MOVE + 9), nullptr)));
}

void CasterDruidAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("medium aoe", NextAction::array(0,
            new NextAction("starfall", ACTION_HIGH + 4),
            new NextAction("wild mushroom", ACTION_HIGH + 3),
            new NextAction("wild mushroom: detonate", ACTION_HIGH + 2),
            new NextAction("astral storm", ACTION_HIGH + 1),
            new NextAction("hurricane", ACTION_NORMAL + 1),
            nullptr)));
    triggers.push_back(new TriggerNode(
        "light aoe", NextAction::array(0,
            new NextAction("sunfire on attacker", ACTION_NORMAL + 4),
            new NextAction("moonfire on attacker", ACTION_NORMAL + 3),
            new NextAction("insect swarm on attacker", ACTION_NORMAL + 2),
            nullptr)));
}

void CasterDruidDebuffStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("faerie fire", NextAction::array(0, new NextAction("faerie fire", ACTION_HIGH), nullptr)));
}
