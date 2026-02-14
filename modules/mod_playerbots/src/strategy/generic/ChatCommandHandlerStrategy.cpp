#include "ChatCommandHandlerStrategy.h"

#include "Playerbots.h"

ChatCommandHandlerStrategy::ChatCommandHandlerStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

void ChatCommandHandlerStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("lfg", NextAction::array(0, new NextAction("lfg", relevance), nullptr)));
    triggers.push_back(new TriggerNode("summon", NextAction::array(0, new NextAction("summon", relevance), nullptr)));
    triggers.push_back(new TriggerNode("group summon", NextAction::array(0, new NextAction("summon", relevance), nullptr)));
    triggers.push_back(new TriggerNode("revive", NextAction::array(0, new NextAction("spirit healer", relevance), nullptr)));
    triggers.push_back(new TriggerNode("group revive", NextAction::array(0, new NextAction("spirit healer", relevance), nullptr)));
    triggers.push_back(new TriggerNode("maintenance", NextAction::array(0, new NextAction("maintenance", relevance), nullptr)));
    triggers.push_back(new TriggerNode("bank", NextAction::array(0, new NextAction("bank", relevance), nullptr)));
    triggers.push_back(new TriggerNode("gb", NextAction::array(0, new NextAction("gb", relevance), nullptr)));
    triggers.push_back(new TriggerNode("guild bank", NextAction::array(0, new NextAction("guild bank", relevance), nullptr)));
    triggers.push_back(new TriggerNode("guild invite", NextAction::array(0, new NextAction("guild invite", relevance), nullptr)));
    triggers.push_back(new TriggerNode("guild join", NextAction::array(0, new NextAction("guild join", relevance), nullptr)));
    triggers.push_back(new TriggerNode("guild promote", NextAction::array(0, new NextAction("guild promote", relevance), nullptr)));
    triggers.push_back(new TriggerNode("guild demote", NextAction::array(0, new NextAction("guild demote", relevance), nullptr)));
    triggers.push_back(new TriggerNode("guild remove", NextAction::array(0, new NextAction("guild remove", relevance), nullptr)));
    triggers.push_back(new TriggerNode("guild leader", NextAction::array(0, new NextAction("guild leader", relevance), nullptr)));
    triggers.push_back(new TriggerNode("guild leave", NextAction::array(0, new NextAction("guild leave", relevance), nullptr)));
    triggers.push_back(new TriggerNode("mail", NextAction::array(0, new NextAction("mail", relevance), nullptr)));
    triggers.push_back(new TriggerNode("sendmail", NextAction::array(0, new NextAction("sendmail", relevance), nullptr)));
    triggers.push_back(new TriggerNode("ah", NextAction::array(0, new NextAction("ah", relevance), nullptr)));
    triggers.push_back(new TriggerNode("help", NextAction::array(0, new NextAction("help", relevance), nullptr)));
    triggers.push_back(new TriggerNode("reset", NextAction::array(0, new NextAction("reset", relevance), nullptr)));
    triggers.push_back(new TriggerNode("reset ai", NextAction::array(0, new NextAction("reset ai", relevance), nullptr)));
    triggers.push_back(new TriggerNode("reset ai soft", NextAction::array(0, new NextAction("reset ai soft", relevance), nullptr)));
    triggers.push_back(new TriggerNode("reset strats", NextAction::array(0, new NextAction("reset strats", relevance), nullptr)));
    triggers.push_back(new TriggerNode("reset values", NextAction::array(0, new NextAction("reset values", relevance), nullptr)));
    triggers.push_back(new TriggerNode("save ai", NextAction::array(0, new NextAction("save ai", relevance), nullptr)));
    triggers.push_back(new TriggerNode("load ai", NextAction::array(0, new NextAction("load ai", relevance), nullptr)));
    triggers.push_back(new TriggerNode("list ai", NextAction::array(0, new NextAction("list ai", relevance), nullptr)));
    triggers.push_back(new TriggerNode("cs", NextAction::array(0, new NextAction("cs", relevance), nullptr)));
    triggers.push_back(new TriggerNode("q", NextAction::array(0, new NextAction("query quest", relevance), nullptr)));
    triggers.push_back(new TriggerNode("quests", NextAction::array(0, new NextAction("list quests", relevance), nullptr)));
    triggers.push_back(new TriggerNode("drop", NextAction::array(0, new NextAction("drop quest", relevance), nullptr)));
    triggers.push_back(new TriggerNode("share", NextAction::array(0, new NextAction("share quest", relevance), nullptr)));
}
