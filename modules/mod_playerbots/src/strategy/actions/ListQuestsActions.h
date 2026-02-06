#ifndef _PLAYERBOT_LISTQUESTSACTION_H
#define _PLAYERBOT_LISTQUESTSACTION_H

#include "Action.h"

class PlayerbotAI;

enum QuestListFilter
{
    QUEST_LIST_FILTER_SUMMARY = 0x00,
    QUEST_LIST_FILTER_INCOMPLETED = 0x01,
    QUEST_LIST_FILTER_COMPLETED = 0x02,
    QUEST_LIST_FILTER_ALL = QUEST_LIST_FILTER_INCOMPLETED | QUEST_LIST_FILTER_COMPLETED
};

enum QuestTravelDetail
{
    QUEST_TRAVEL_DETAIL_NONE = 0,
    QUEST_TRAVEL_DETAIL_SUMMARY,
    QUEST_TRAVEL_DETAIL_FULL
};

class ListQuestsAction : public Action
{
public:
    ListQuestsAction(PlayerbotAI* botAI) : Action(botAI, "list quests") {}

    bool Execute(Event event) override;

private:
    void ListQuests(QuestListFilter filter, QuestTravelDetail travelDetail = QUEST_TRAVEL_DETAIL_NONE);
    uint32 ListQuests(bool completed, bool silent, QuestTravelDetail travelDetail);
};

#endif
