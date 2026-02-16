

#include "DpsHunterStrategy.h"

#include "Playerbots.h"

class DpsHunterStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DpsHunterStrategyActionNodeFactory()
    {
        creators["aimed shot"] = &aimed_shot; 
        creators["steady shot"] = &steady_shot;
        creators["cobra shot"] = &cobra_shot;
    }

private:
    static ActionNode* aimed_shot([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("aimed shot",
                              /*P*/ nullptr,
                              /*A*/ NextAction::array(0, new NextAction("multi-shot"), nullptr),
                              /*C*/ nullptr);
    }
    static ActionNode* steady_shot([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("steady shot",
                              /*P*/ nullptr,
                              /*A*/ NextAction::array(0, new NextAction("arcane shot"), nullptr),
                              /*C*/ nullptr);
    }
    static ActionNode* cobra_shot([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("cobra shot",
                              /*P*/ nullptr,
                              /*A*/ NextAction::array(0, new NextAction("arcane shot"), nullptr),
                              /*C*/ nullptr);
    }
};

DpsHunterStrategy::DpsHunterStrategy(PlayerbotAI* botAI) : GenericHunterStrategy(botAI)
{
    actionNodeFactories.Add(new DpsHunterStrategyActionNodeFactory());
}

NextAction** DpsHunterStrategy::getDefaultActions()
{
    return NextAction::array(
        0,
        new NextAction("kill shot", ACTION_DEFAULT + 0.9f),
        new NextAction("explosive shot", ACTION_DEFAULT + 0.85f),
        new NextAction("kill command", ACTION_DEFAULT + 0.82f),
        new NextAction("chimera shot", ACTION_DEFAULT + 0.8f),
        new NextAction("black arrow", ACTION_DEFAULT + 0.78f),
        new NextAction("a murder of crows", ACTION_DEFAULT + 0.76f),
        new NextAction("glaive toss", ACTION_DEFAULT + 0.74f),
        new NextAction("dire beast", ACTION_DEFAULT + 0.72f),
        new NextAction("aimed shot", ACTION_DEFAULT + 0.6f),
        new NextAction("arcane shot", ACTION_DEFAULT + 0.5f),
        new NextAction("cobra shot", ACTION_DEFAULT + 0.4f),
        new NextAction("steady shot", ACTION_DEFAULT + 0.3f),
        new NextAction("auto shot", ACTION_DEFAULT), nullptr);
}

void DpsHunterStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericHunterStrategy::InitTriggers(triggers);

    triggers.push_back(
        new TriggerNode("target critical health", NextAction::array(0, new NextAction("kill shot", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(
        new TriggerNode("black arrow", NextAction::array(0, new NextAction("black arrow", 19.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("focus fire", NextAction::array(0, new NextAction("focus fire", 19.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("hunter's mark", NextAction::array(0, new NextAction("hunter's mark", 31.0f), nullptr)));
    triggers.push_back(new TriggerNode("concussive shot on snare target",
                                       NextAction::array(0, new NextAction("concussive shot", 20.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("has aggro", NextAction::array(0, new NextAction("concussive shot", 20.0f), nullptr)));
}

void DpsAoeHunterStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("light aoe", NextAction::array(0, new NextAction("multi-shot", 20.0f), nullptr)));
    triggers.push_back(new TriggerNode("medium aoe", NextAction::array(0, new NextAction("barrage", 22.0f), nullptr)));
    triggers.push_back(new TriggerNode("medium aoe", NextAction::array(0, new NextAction("explosive trap", 21.0f), nullptr)));
    triggers.push_back(
        new TriggerNode("serpent sting on attacker",
                        NextAction::array(0, new NextAction("serpent sting on attacker", 17.0f), nullptr)));
}

void DpsHunterDebuffStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("no stings", NextAction::array(0, new NextAction("serpent sting", 18.0f), nullptr)));
}
