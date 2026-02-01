#include "DisciplinePriestStrategy.h"

#include "Playerbots.h"

DisciplinePriestStrategy::DisciplinePriestStrategy(PlayerbotAI* botAI) : HealPriestStrategy(botAI) {}

NextAction** DisciplinePriestStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("penance", ACTION_DEFAULT + 0.6f),
                             new NextAction("power word: solace", ACTION_DEFAULT + 0.5f),
                             new NextAction("holy fire", ACTION_DEFAULT + 0.4f),
                             new NextAction("smite", ACTION_DEFAULT + 0.3f),
                             new NextAction("mind blast", ACTION_DEFAULT + 0.2f),
                             new NextAction("shoot", ACTION_DEFAULT), nullptr);
}

void DisciplinePriestStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    HealPriestStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("inner fire", NextAction::array(0, new NextAction("inner fire", ACTION_HIGH), nullptr)));
    triggers.push_back(new TriggerNode("evangelism", NextAction::array(0, new NextAction("archangel", ACTION_HIGH + 6), nullptr)));

    triggers.push_back(new TriggerNode("low mana",
                                       NextAction::array(0,
                                                        new NextAction("mindbender", ACTION_HIGH + 6),
                                                        new NextAction("shadowfiend", ACTION_HIGH + 5),
                                                        new NextAction("power word: solace", ACTION_HIGH + 4),
                                                        nullptr)));

    triggers.push_back(new TriggerNode(
        "medium group heal setting",
        NextAction::array(0,
                          new NextAction("spirit shell", ACTION_CRITICAL_HEAL + 7),
                          new NextAction("divine star", ACTION_CRITICAL_HEAL + 6),
                          new NextAction("cascade", ACTION_CRITICAL_HEAL + 6),
                          new NextAction("halo", ACTION_CRITICAL_HEAL + 6),
                          new NextAction("prayer of healing on party", ACTION_CRITICAL_HEAL + 5),
                          nullptr)));
}
