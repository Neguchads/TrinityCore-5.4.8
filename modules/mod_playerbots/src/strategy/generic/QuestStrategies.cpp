#include "QuestStrategies.h"
#include "Playerbots.h"

void QuestStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    PassTroughStrategy::InitTriggers(triggers);

    triggers.push_back(
        new TriggerNode("quest share", NextAction::array(0, new NextAction("accept quest share", relevance), nullptr)));
    triggers.push_back(
        new TriggerNode("confirm quest", NextAction::array(0, new NextAction("confirm quest", relevance), nullptr)));
}

void DefaultQuestStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    QuestStrategy::InitTriggers(triggers);

    triggers.push_back(
        new TriggerNode("use game object", NextAction::array(0, new NextAction("talk to quest giver", relevance), nullptr)));
    triggers.push_back(
        new TriggerNode("gossip hello", NextAction::array(0, new NextAction("talk to quest giver", relevance), nullptr)));
    triggers.push_back(
        new TriggerNode("complete quest", NextAction::array(0, new NextAction("talk to quest giver", relevance), nullptr)));
}

void AcceptAllQuestsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    QuestStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("use game object",
        NextAction::array(0, new NextAction("talk to quest giver", relevance), new NextAction("accept all quests", relevance), nullptr)));
    triggers.push_back(new TriggerNode("gossip hello",
        NextAction::array(0, new NextAction("talk to quest giver", relevance), new NextAction("accept all quests", relevance), nullptr)));
    triggers.push_back(new TriggerNode("complete quest",
        NextAction::array(0, new NextAction("talk to quest giver", relevance), new NextAction("accept all quests", relevance), nullptr)));
}
