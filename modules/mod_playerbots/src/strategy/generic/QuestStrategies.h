#ifndef _PLAYERBOT_QUESTSTRATEGIES_H
#define _PLAYERBOT_QUESTSTRATEGIES_H

#include "PassTroughStrategy.h"

class PlayerbotAI;

class QuestStrategy : public PassTroughStrategy
{
public:
    QuestStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class DefaultQuestStrategy : public QuestStrategy
{
public:
    DefaultQuestStrategy(PlayerbotAI* botAI) : QuestStrategy(botAI) {}

    std::string const getName() override { return "quest"; }
};

class AcceptAllQuestsStrategy : public QuestStrategy
{
public:
    AcceptAllQuestsStrategy(PlayerbotAI* botAI) : QuestStrategy(botAI) {}

    std::string const getName() override { return "accept all quests"; }
};

#endif
