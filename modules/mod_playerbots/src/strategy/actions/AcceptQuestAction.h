#ifndef _PLAYERBOT_ACCEPTQUESTACTION_H
#define _PLAYERBOT_ACCEPTQUESTACTION_H

#include "QuestAction.h"

class AcceptQuestAction : public QuestAction
{
public:
    AcceptQuestAction(PlayerbotAI* botAI) : QuestAction(botAI, "accept quest") {}
    bool Execute(Event event) override;
};

class AcceptAllQuestsAction : public QuestAction
{
public:
    AcceptAllQuestsAction(PlayerbotAI* botAI) : QuestAction(botAI, "accept all quests") {}
    bool Execute(Event event) override;
};

class TalkToQuestGiverAction : public QuestAction
{
public:
    TalkToQuestGiverAction(PlayerbotAI* botAI) : QuestAction(botAI, "talk to quest giver") {}
    bool Execute(Event event) override;
};

class TurnInQueryQuestAction : public TalkToQuestGiverAction
{
public:
    TurnInQueryQuestAction(PlayerbotAI* botAI) : TalkToQuestGiverAction(botAI) {}
    bool Execute(Event event) override;
};

class AcceptQuestShareAction : public Action
{
public:
    AcceptQuestShareAction(PlayerbotAI* botAI) : Action(botAI, "accept quest share") {}
    bool Execute(Event event) override;
};

class ConfirmQuestAction : public Action
{
public:
    ConfirmQuestAction(PlayerbotAI* botAI) : Action(botAI, "confirm quest") {}
    bool Execute(Event event) override;
};

#endif
