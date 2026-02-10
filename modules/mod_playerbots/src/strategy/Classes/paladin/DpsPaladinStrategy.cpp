

#include "DpsPaladinStrategy.h"

#include "Playerbots.h"
#include "Strategy.h"

class DpsPaladinStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DpsPaladinStrategyActionNodeFactory()
    {
        
    }

private:
    
};

DpsPaladinStrategy::DpsPaladinStrategy(PlayerbotAI* botAI) : GenericPaladinStrategy(botAI)
{
    actionNodeFactories.Add(new DpsPaladinStrategyActionNodeFactory());
}

NextAction** DpsPaladinStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("hammer of wrath", ACTION_DEFAULT + 0.7f),
                             new NextAction("divine storm", ACTION_DEFAULT + 0.6f),
                             new NextAction("crusader strike", ACTION_DEFAULT + 0.5f),
                             new NextAction("judgement", ACTION_DEFAULT + 0.4f),
                             new NextAction("exorcism", ACTION_DEFAULT + 0.3f),
                             new NextAction("holy wrath", ACTION_DEFAULT + 0.2f),
                             new NextAction("consecration", ACTION_DEFAULT + 0.1f),
                             new NextAction("melee", ACTION_DEFAULT),
                             nullptr);
}

void DpsPaladinStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericPaladinStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("seal", NextAction::array(0, new NextAction("seal of righteousness", ACTION_HIGH), nullptr)));
    triggers.push_back(new TriggerNode("avenging wrath", NextAction::array(0, new NextAction("avenging wrath", ACTION_HIGH + 6), nullptr)));
    triggers.push_back(new TriggerNode("enemy out of melee", NextAction::array(0, new NextAction("reach melee", ACTION_HIGH + 1), NULL)));
    triggers.push_back(new TriggerNode("target critical health", NextAction::array(0, new NextAction("hammer of wrath", ACTION_HIGH + 7), nullptr)));
}
