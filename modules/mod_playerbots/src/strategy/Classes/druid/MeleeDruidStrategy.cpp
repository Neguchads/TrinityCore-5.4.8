

#include "MeleeDruidStrategy.h"

#include "Playerbots.h"

MeleeDruidStrategy::MeleeDruidStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

NextAction** MeleeDruidStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("faerie fire", ACTION_DEFAULT + 0.1f),
                             new NextAction("melee", ACTION_DEFAULT), nullptr);
}

void MeleeDruidStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "omen of clarity", NextAction::array(0, new NextAction("omen of clarity", ACTION_HIGH + 9), nullptr)));

    triggers.push_back(new TriggerNode("enemy out of melee",
        NextAction::array(0, new NextAction("reach melee", ACTION_HIGH + 1), nullptr)));

    CombatStrategy::InitTriggers(triggers);
}
