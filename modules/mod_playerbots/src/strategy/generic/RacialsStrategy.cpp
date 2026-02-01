#include "RacialsStrategy.h"

#include "Playerbots.h"

class RacialsStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    RacialsStrategyActionNodeFactory() { creators["lifeblood"] = &lifeblood; }

private:
    static ActionNode* lifeblood(PlayerbotAI* botAI)
    {
        return new ActionNode("lifeblood",
                              /*P*/ nullptr,
                              /*A*/ NextAction::array(0, new NextAction("gift of the naaru"), nullptr),
                              /*C*/ nullptr);
    }
};

RacialsStrategy::RacialsStrategy(PlayerbotAI* botAI) : Strategy(botAI)
{
    actionNodeFactories.Add(new RacialsStrategyActionNodeFactory());
}

void RacialsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("low health", NextAction::array(0, new NextAction("lifeblood", ACTION_NORMAL + 5), nullptr)));
    triggers.push_back(new TriggerNode("medium aoe", NextAction::array(0, new NextAction("war stomp", ACTION_NORMAL + 5), nullptr)));
    triggers.push_back(new TriggerNode("low mana", NextAction::array(0, new NextAction("arcane torrent", ACTION_NORMAL + 5), nullptr)));

    triggers.push_back(new TriggerNode("generic boost",
        NextAction::array(0,
            new NextAction("blood fury", ACTION_NORMAL + 5),
            new NextAction("berserking", ACTION_NORMAL + 5),
            new NextAction("use trinket", ACTION_NORMAL + 4),
            nullptr)));
}
