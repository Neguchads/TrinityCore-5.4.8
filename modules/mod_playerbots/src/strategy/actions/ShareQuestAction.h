#ifndef _PLAYERBOT_SHAREQUESTACTION_H
#define _PLAYERBOT_SHAREQUESTACTION_H

#include "Action.h"

class PlayerbotAI;

class ShareQuestAction : public Action
{
public:
    ShareQuestAction(PlayerbotAI* botAI) : Action(botAI, "share quest") {}

    bool Execute(Event event) override;
};

class AutoShareQuestAction : public Action
{
public:
    AutoShareQuestAction(PlayerbotAI* botAI) : Action(botAI, "auto share quest") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

#endif
