

#include "ArmsWarriorStrategy.h"

#include "Playerbots.h"

class ArmsWarriorStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    ArmsWarriorStrategyActionNodeFactory()
    {
        creators["mortal strike"] = &mortal_strike;
        creators["slam"] = &slam;

        creators["overpower"] = &overpower;
        creators["sweeping strikes"] = &sweeping_strikes;
        creators["demoralizing banner"] = &demoralizing_banner;
    }

private:
    ACTION_NODE(mortal_strike, "mortal strike");
    ACTION_NODE(slam, "slam");
    ACTION_NODE(overpower, "overpower");
    ACTION_NODE(sweeping_strikes, "sweeping strikes");
    ACTION_NODE(demoralizing_banner, "demoralizing banner");
};

ArmsWarriorStrategy::ArmsWarriorStrategy(PlayerbotAI* botAI) : GenericWarriorStrategy(botAI)
{
    actionNodeFactories.Add(new ArmsWarriorStrategyActionNodeFactory());
}

NextAction** ArmsWarriorStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("bloodbath", ACTION_DEFAULT + 0.8f),
        new NextAction("colossus smash", ACTION_DEFAULT + 0.6f),
        new NextAction("mortal strike", ACTION_DEFAULT + 0.5f),
        new NextAction("overpower", ACTION_DEFAULT + 0.4f),
        new NextAction("slam", ACTION_DEFAULT + 0.3f),
        new NextAction("execute", ACTION_DEFAULT + 0.2f),
        new NextAction("melee", ACTION_DEFAULT),
        nullptr);
}

void ArmsWarriorStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarriorStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("battle shout", NextAction::array(0, new NextAction("battle shout", ACTION_HIGH + 9), nullptr)));
    triggers.push_back(new TriggerNode("recklessness", NextAction::array(0, new NextAction("recklessness", ACTION_HIGH + 8), nullptr)));
    triggers.push_back(new TriggerNode("high rage available",NextAction::array(0, new NextAction("slam", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode("mortal wounds", NextAction::array(0, new NextAction("mortal strike", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(new TriggerNode("overpower", NextAction::array(0, new NextAction("overpower", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(new TriggerNode("taste for blood", NextAction::array(0, new NextAction("overpower", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(new TriggerNode("colossus smash", NextAction::array(0, new NextAction("colossus smash", ACTION_HIGH + 6), nullptr)));
    triggers.push_back(new TriggerNode("target critical health", NextAction::array(0, new NextAction("execute", ACTION_HIGH + 7), nullptr)));
    triggers.push_back(new TriggerNode("medium aoe", NextAction::array(0, new NextAction("sweeping strikes", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode("high aoe", NextAction::array(0, new NextAction("bladestorm", ACTION_HIGH + 4), nullptr)));
}
