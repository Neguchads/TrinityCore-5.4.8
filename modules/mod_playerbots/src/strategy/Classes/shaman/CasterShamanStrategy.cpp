

#include "CasterShamanStrategy.h"

#include "Playerbots.h"

class CasterShamanStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    CasterShamanStrategyActionNodeFactory()
    {
        creators["magma totem"] = &magma_totem;
        creators["totem of wrath"] = &totem_of_wrath;
    }

private:
    static ActionNode* magma_totem([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("magma totem",
                              /*P*/ nullptr,
                              /*A*/ nullptr,
                              /*C*/ NextAction::array(0, new NextAction("fire nova"), nullptr));
    }
    static ActionNode* totem_of_wrath(PlayerbotAI* botAI)
    {
        return new ActionNode("totem of wrath",
                              /*P*/ NULL,
                              /*A*/ NextAction::array(0, new NextAction("flametongue totem"), NULL),
                              /*C*/ NULL);
    }
};

CasterShamanStrategy::CasterShamanStrategy(PlayerbotAI* botAI) : GenericShamanStrategy(botAI)
{
    actionNodeFactories.Add(new CasterShamanStrategyActionNodeFactory());
}

NextAction** CasterShamanStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("flame shock", ACTION_DEFAULT + 0.5f),
                             new NextAction("lava burst", ACTION_DEFAULT + 0.4f),
                             new NextAction("chain lightning", ACTION_DEFAULT + 0.3f),
                             new NextAction("lightning bolt", ACTION_DEFAULT + 0.2f),
                             new NextAction("earth shock", ACTION_DEFAULT + 0.1f), // cast during movement
                             nullptr);
}

void CasterShamanStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);
    
    // triggers.push_back(new TriggerNode("enemy out of spell", NextAction::array(0, new NextAction("reach spell",
    // ACTION_NORMAL + 9), nullptr))); triggers.push_back(new TriggerNode("shaman weapon", NextAction::array(0, new
    // NextAction("flametongue weapon", 23.0f), nullptr)));
    triggers.push_back(new TriggerNode(
        "enough mana", NextAction::array(0, new NextAction("chain lightning", ACTION_DEFAULT + 0.1f), nullptr)));
        
    triggers.push_back(new TriggerNode("main hand weapon no imbue",
                                       NextAction::array(0, new NextAction("flametongue weapon", 22.0f), nullptr)));
    // triggers.push_back(new TriggerNode("searing totem", NextAction::array(0, new NextAction("searing totem", 19.0f),
    // nullptr)));
    triggers.push_back(
        new TriggerNode("flame shock", NextAction::array(0, new NextAction("flame shock", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("elemental mastery",
                                       NextAction::array(0, new NextAction("elemental mastery", 27.0f), nullptr)));
    // triggers.push_back(new TriggerNode("frost shock snare", NextAction::array(0, new NextAction("frost
    // shock", 21.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("no fire totem", NextAction::array(0, new NextAction("searing totem", 15.0f), NULL)));
    triggers.push_back(new TriggerNode("fire elemental totem",
                                       NextAction::array(0, new NextAction("fire elemental totem", 32.0f), nullptr)));
                                       
    triggers.push_back(
        new TriggerNode("medium mana", NextAction::array(0, new NextAction("thunderstorm", ACTION_HIGH + 1), nullptr)));
    

    triggers.push_back(new TriggerNode("enemy is close",
                                       NextAction::array(0, new NextAction("thunderstorm", ACTION_HIGH + 1), nullptr)));

    triggers.push_back(new TriggerNode("enemy too close for spell",
                                       NextAction::array(0, new NextAction("flee", ACTION_MOVE + 9), nullptr)));
}

void CasterAoeShamanStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("light aoe", NextAction::array(0, new NextAction("chain lightning", 25.0f), nullptr)));
    triggers.push_back(new TriggerNode(
        "medium aoe",
        NextAction::array(0, new NextAction("magma totem", 26.0f), new NextAction("fire nova", 25.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("high aoe", NextAction::array(0, new NextAction("earthquake", 27.0f), nullptr)));
}
