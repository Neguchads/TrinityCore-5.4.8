#ifndef _PLAYERBOT_QUESTACTION_H
#define _PLAYERBOT_QUESTACTION_H

#include "Action.h"

class Object;
class ObjectGuid;
class Quest;
class WorldObject;

class QuestAction : public Action
{
public:
    QuestAction(PlayerbotAI* botAI, std::string const& name) : Action(botAI, name) {}

protected:
    bool ProcessNearbyQuestGivers(uint32 questId = 0);
    bool ProcessQuestGiver(ObjectGuid guid, uint32 questId = 0);
    bool ProcessQuestGiver(WorldObject* questGiver, uint32 questId = 0);
    bool AcceptQuestFrom(Object* questGiver, Quest const* quest);
    bool TryCompleteQuest(Quest const* quest, Object* questGiver = nullptr);
    bool TryTurnInQuest(Quest const* quest, Object* questGiver);
};

class QuestUpdateAddKillAction : public Action
{
public:
    QuestUpdateAddKillAction(PlayerbotAI* botAI) : Action(botAI, "quest update add kill") {}
    bool Execute(Event event) override;
};

class QuestUpdateFailedAction : public Action
{
public:
    QuestUpdateFailedAction(PlayerbotAI* botAI) : Action(botAI, "quest update failed") {}
    bool Execute(Event event) override;
};

class QuestUpdateFailedTimerAction : public Action
{
public:
    QuestUpdateFailedTimerAction(PlayerbotAI* botAI) : Action(botAI, "quest update failed timer") {}
    bool Execute(Event event) override;
};

class QuestUpdateCompleteAction : public Action
{
public:
    QuestUpdateCompleteAction(PlayerbotAI* botAI) : Action(botAI, "quest update complete") {}
    bool Execute(Event event) override;
};

class QuestItemPushResultAction : public Action
{
public:
    QuestItemPushResultAction(PlayerbotAI* botAI) : Action(botAI, "quest item push result") {}
    bool Execute(Event event) override;
};

#endif
