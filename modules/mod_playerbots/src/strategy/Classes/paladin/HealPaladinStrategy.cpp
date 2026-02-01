

#include "HealPaladinStrategy.h"

#include "Playerbots.h"
#include "Strategy.h"

class HealPaladinStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    // HealPaladinStrategyActionNodeFactory()
    // {
    //     creators["concentration aura"] = &concentration_aura;
    // }

private:
    // ACTION_NODE_A(concentration_aura, "concentration aura", "devotion aura");
};

HealPaladinStrategy::HealPaladinStrategy(PlayerbotAI* botAI) : GenericPaladinStrategy(botAI)
{
    actionNodeFactories.Add(new HealPaladinStrategyActionNodeFactory());
}

NextAction** HealPaladinStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("holy shock", ACTION_DEFAULT + 0.2f),
                             new NextAction("holy light", ACTION_DEFAULT + 0.1f), nullptr);
}

void HealPaladinStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericPaladinStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("seal", NextAction::array(0, new NextAction("seal of insight", ACTION_HIGH), nullptr)));
    triggers.push_back(new TriggerNode("low mana", NextAction::array(0, new NextAction("divine plea", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode("party member to heal out of spell range", NextAction::array(0, new NextAction("reach party member to heal", ACTION_EMERGENCY + 3), nullptr)));

    triggers.push_back(
        new TriggerNode("medium group heal setting", NextAction::array(0, 
            new NextAction("light's hammer", ACTION_CRITICAL_HEAL + 6),
            new NextAction("holy prism", ACTION_CRITICAL_HEAL + 6),
            new NextAction("light of dawn on party", ACTION_CRITICAL_HEAL + 5),
            new NextAction("holy radiance on party", ACTION_CRITICAL_HEAL + 4),
            new NextAction("avenging wrath", ACTION_HIGH + 4),
            new NextAction("divine favor", ACTION_HIGH + 3),
            nullptr)));

    triggers.push_back(
        new TriggerNode("party member critical health", NextAction::array(0,
            new NextAction("holy shock on party", ACTION_CRITICAL_HEAL + 6),
            new NextAction("eternal flame on party", ACTION_CRITICAL_HEAL + 5),
            new NextAction("word of glory on party", ACTION_CRITICAL_HEAL + 4),
            new NextAction("divine light on party", ACTION_CRITICAL_HEAL + 3),
            new NextAction("flash of light on party", ACTION_CRITICAL_HEAL + 2),
            new NextAction("holy light on party", ACTION_CRITICAL_HEAL + 1), nullptr)));

    triggers.push_back(
        new TriggerNode("party member low health",
                        NextAction::array(0,
                            new NextAction("holy shock on party", ACTION_MEDIUM_HEAL + 6),
                            new NextAction("eternal flame on party", ACTION_MEDIUM_HEAL + 5),
                            new NextAction("word of glory on party", ACTION_MEDIUM_HEAL + 4),
                            new NextAction("divine light on party", ACTION_MEDIUM_HEAL + 3),
                            new NextAction("holy light on party", ACTION_MEDIUM_HEAL + 2), nullptr)));

    triggers.push_back(
        new TriggerNode("party member medium health",
                        NextAction::array(0, new NextAction("holy shock on party", ACTION_LIGHT_HEAL + 10),
                                          new NextAction("holy light on party", ACTION_LIGHT_HEAL + 9),
                                          new NextAction("flash of light on party", ACTION_LIGHT_HEAL + 8), nullptr)));

    triggers.push_back(new TriggerNode(
        "party member almost full health",
        NextAction::array(0, new NextAction("holy light on party", ACTION_LIGHT_HEAL + 3), nullptr)));

    triggers.push_back(new TriggerNode(
        "beacon of light on main tank",
        NextAction::array(0, new NextAction("beacon of light on main tank", ACTION_CRITICAL_HEAL + 7), nullptr)));

    triggers.push_back(new TriggerNode(
        "sacred shield on main tank",
        NextAction::array(0, new NextAction("sacred shield on main tank", ACTION_CRITICAL_HEAL + 6), nullptr)));
}
