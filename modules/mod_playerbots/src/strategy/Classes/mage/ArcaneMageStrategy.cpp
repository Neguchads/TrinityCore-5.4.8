

#include "ArcaneMageStrategy.h"

#include "Playerbots.h"

class ArcaneMageStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    ArcaneMageStrategyActionNodeFactory()
    {
        creators["arcane blast"] = &arcane_blast;
        creators["arcane barrage"] = &arcane_barrage;
        creators["slow"] = &slow;
        creators["nether tempest"] = &nether_tempest;
        creators["living bomb"] = &living_bomb;
    }

private:
    static ActionNode* arcane_blast([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("arcane blast",
                              /*P*/ nullptr,
                              /*A*/ NextAction::array(0, new NextAction("arcane missiles"), nullptr),
                              /*C*/ nullptr);
    }

    static ActionNode* arcane_barrage([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("arcane barrage",
                              /*P*/ nullptr,
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* slow([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("slow",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }

    static ActionNode* nether_tempest([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("nether tempest",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
    static ActionNode* living_bomb([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("living bomb",
            /*P*/ nullptr,
            /*A*/ nullptr,
            /*C*/ nullptr);
    }
};

ArcaneMageStrategy::ArcaneMageStrategy(PlayerbotAI* botAI) : GenericMageStrategy(botAI)
{
    actionNodeFactories.Add(new ArcaneMageStrategyActionNodeFactory());
}

NextAction** ArcaneMageStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("arcane power", ACTION_DEFAULT + 0.9f),
        new NextAction("nether tempest", ACTION_DEFAULT + 0.7f),
        new NextAction("living bomb", ACTION_DEFAULT + 0.65f),
        new NextAction("arcane missiles", ACTION_DEFAULT + 0.6f),
        new NextAction("arcane barrage", ACTION_DEFAULT + 0.5f),
        new NextAction("arcane blast", ACTION_DEFAULT + 0.4f),
        new NextAction("fire blast", ACTION_DEFAULT + 0.2f), // cast during movement
        new NextAction("shoot", ACTION_DEFAULT), nullptr);
}

void ArcaneMageStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMageStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("arcane blast stack", NextAction::array(0, new NextAction("arcane missiles", 15.0f), nullptr)));
    triggers.push_back(new TriggerNode("arcane missiles", NextAction::array(0, new NextAction("arcane missiles", 15.0f), nullptr)));
    triggers.push_back(new TriggerNode("arcane charge", NextAction::array(0, new NextAction("arcane barrage", 15.0f), nullptr)));
    triggers.push_back(new TriggerNode("slow not on target", NextAction::array(0, new NextAction("slow", ACTION_NORMAL + 1), nullptr)));
    triggers.push_back(new TriggerNode("medium health", NextAction::array(0, new NextAction("ice barrier", ACTION_NORMAL), nullptr)));
    triggers.push_back(new TriggerNode("nether tempest", NextAction::array(0, new NextAction("nether tempest", 19.0f), nullptr)));
    triggers.push_back(new TriggerNode("living bomb", NextAction::array(0, new NextAction("living bomb", 18.0f), nullptr)));
    triggers.push_back(new TriggerNode("arcane power", NextAction::array(0, new NextAction("arcane power", 19.0f), nullptr)));
    triggers.push_back(new TriggerNode("rune of power", NextAction::array(0, new NextAction("rune of power", 18.0f), nullptr)));
}

void ArcaneMageAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("medium aoe", NextAction::array(0,
        new NextAction("nether tempest on attackers", 24.0f),
        new NextAction("arcane explosion", 23.5f),
        new NextAction("blizzard", 23.0f), nullptr)));

    triggers.push_back(new TriggerNode("frost nova on target", NextAction::array(0,
        new NextAction("flamestrike", ACTION_HIGH + 2),
        new NextAction("cone of cold", ACTION_HIGH + 1), nullptr)));
}
