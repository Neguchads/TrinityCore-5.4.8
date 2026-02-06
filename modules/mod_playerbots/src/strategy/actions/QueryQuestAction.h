#ifndef _PLAYERBOT_QUERYQUESTACTION_H
#define _PLAYERBOT_QUERYQUESTACTION_H

#include "Action.h"

class PlayerbotAI;

class QueryQuestAction : public Action
{
public:
    QueryQuestAction(PlayerbotAI* botAI) : Action(botAI, "query quest") {}

    bool Execute(Event event) override;

private:
    void TellObjective(std::string const& name, uint32 available, uint32 required);
    void TellObjectives(uint32 questId);
};

#endif
