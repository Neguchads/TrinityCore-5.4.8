

#include "HolyPriestStrategy.h"

#include "Playerbots.h"

HolyPriestStrategy::HolyPriestStrategy(PlayerbotAI* botAI) : HealPriestStrategy(botAI)
{
}

NextAction** HolyPriestStrategy::getDefaultActions()
{
    return NextAction::array(0,
                             new NextAction("prayer of mending on party", ACTION_DEFAULT + 0.3f),
                             new NextAction("renew on party", ACTION_DEFAULT + 0.2f),
                             new NextAction("heal on party", ACTION_DEFAULT + 0.1f),
                             nullptr);
}

void HolyPriestStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    HealPriestStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("inner fire", NextAction::array(0, new NextAction("inner fire", ACTION_HIGH), nullptr)));

    triggers.push_back(new TriggerNode(
        "medium group heal setting",
        NextAction::array(0,
                          new NextAction("chakra: sanctuary", ACTION_HIGH + 6),
                          new NextAction("holy word: sanctuary", ACTION_CRITICAL_HEAL + 5),
                          new NextAction("circle of healing on party", ACTION_CRITICAL_HEAL + 4),
                          new NextAction("divine star", ACTION_CRITICAL_HEAL + 4),
                          new NextAction("cascade", ACTION_CRITICAL_HEAL + 4),
                          new NextAction("halo", ACTION_CRITICAL_HEAL + 4),
                          nullptr)));

    triggers.push_back(new TriggerNode(
        "party member critical health",
        NextAction::array(0,
                          new NextAction("chakra: serenity", ACTION_HIGH + 6),
                          new NextAction("holy word: serenity", ACTION_CRITICAL_HEAL + 5),
                          nullptr)));

    triggers.push_back(
        new TriggerNode("holy fire", NextAction::array(0, new NextAction("holy fire", ACTION_NORMAL + 9), nullptr)));
    triggers.push_back(
        new TriggerNode("low mana", NextAction::array(0, new NextAction("mindbender", ACTION_HIGH + 6),
                                                     new NextAction("shadowfiend", ACTION_HIGH + 5), nullptr)));
}

HolyHealPriestStrategy::HolyHealPriestStrategy(PlayerbotAI* botAI) : GenericPriestStrategy(botAI)
{
    actionNodeFactories.Add(new GenericPriestStrategyActionNodeFactory());
}

NextAction** HolyHealPriestStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("prayer of mending on party", ACTION_DEFAULT + 0.2f),
                             new NextAction("renew on party", ACTION_DEFAULT + 0.1f), nullptr);
}

void HolyHealPriestStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericPriestStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("inner fire", NextAction::array(0, new NextAction("inner fire", ACTION_HIGH), nullptr)));

    triggers.push_back(
        new TriggerNode("group heal setting",
                        NextAction::array(0, 
                            new NextAction("prayer of mending on party", ACTION_MEDIUM_HEAL + 9),
                            new NextAction("circle of healing on party", ACTION_MEDIUM_HEAL + 8), nullptr)));

    triggers.push_back(new TriggerNode(
        "medium group heal setting",
        NextAction::array(0, new NextAction("divine hymn", ACTION_CRITICAL_HEAL + 7),
                          new NextAction("chakra: sanctuary", ACTION_HIGH + 6),
                          new NextAction("holy word: sanctuary", ACTION_CRITICAL_HEAL + 6),
                          new NextAction("divine star", ACTION_CRITICAL_HEAL + 6),
                          new NextAction("cascade", ACTION_CRITICAL_HEAL + 6),
                          new NextAction("halo", ACTION_CRITICAL_HEAL + 6),
                          new NextAction("prayer of mending on party", ACTION_CRITICAL_HEAL + 6),
                          new NextAction("circle of healing on party", ACTION_CRITICAL_HEAL + 5),
                          new NextAction("prayer of healing on party", ACTION_CRITICAL_HEAL + 4), nullptr)));

    triggers.push_back(new TriggerNode(
        "party member critical health",
        NextAction::array(0, 
                          new NextAction("guardian spirit on party", ACTION_CRITICAL_HEAL + 6),
                          new NextAction("chakra: serenity", ACTION_HIGH + 6),
                          new NextAction("holy word: serenity", ACTION_CRITICAL_HEAL + 5),
                          new NextAction("power word: shield on party", ACTION_CRITICAL_HEAL + 5),
                          new NextAction("prayer of mending on party", ACTION_CRITICAL_HEAL + 3),
                          new NextAction("greater heal on party", ACTION_MEDIUM_HEAL + 2),
                          new NextAction("flash heal on party", ACTION_CRITICAL_HEAL + 1),
                          nullptr)));

    triggers.push_back(
        new TriggerNode("party member low health",
                        NextAction::array(0, new NextAction("circle of healing on party", ACTION_MEDIUM_HEAL + 4),
                                          new NextAction("prayer of mending on party", ACTION_MEDIUM_HEAL + 3),
                                          new NextAction("greater heal on party", ACTION_MEDIUM_HEAL + 2),
                                          new NextAction("flash heal on party", ACTION_MEDIUM_HEAL + 1), nullptr)));

    triggers.push_back(
        new TriggerNode("party member medium health",
                        NextAction::array(0, new NextAction("circle of healing on party", ACTION_LIGHT_HEAL + 7),
                                          new NextAction("prayer of mending on party", ACTION_LIGHT_HEAL + 6),
                                          new NextAction("greater heal on party", ACTION_MEDIUM_HEAL + 5),
                                          new NextAction("flash heal on party", ACTION_LIGHT_HEAL + 4),
                                          // new NextAction("renew on party", ACTION_LIGHT_HEAL + 8),
                                          nullptr)));

    triggers.push_back(
        new TriggerNode("party member almost full health",
                        NextAction::array(0,
                                          new NextAction("renew on party", ACTION_LIGHT_HEAL + 2),
                                          new NextAction("prayer of mending on party", ACTION_LIGHT_HEAL + 1),
                                          nullptr)));

    triggers.push_back(new TriggerNode(
        "party member to heal out of spell range",
        NextAction::array(0, new NextAction("reach party member to heal", ACTION_CRITICAL_HEAL + 10), nullptr)));
}
