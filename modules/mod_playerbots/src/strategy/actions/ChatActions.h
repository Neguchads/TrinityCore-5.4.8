#ifndef _PLAYERBOT_CHATACTIONS_H
#define _PLAYERBOT_CHATACTIONS_H

#include "Action.h"
#include "NamedObjectContext.h"

class PlayerbotAI;

class SayAction : public Action, public Qualified
{
public:
    SayAction(PlayerbotAI* botAI);
    bool Execute(Event event) override;
    bool isUseful() override;
};

class TalkAction : public Action
{
public:
    TalkAction(PlayerbotAI* botAI);
    bool Execute(Event event) override;
    bool isUseful() override;
};

class EmoteAction : public Action
{
public:
    EmoteAction(PlayerbotAI* botAI);
    bool Execute(Event event) override;
    bool isUseful() override;
};

class GreetAction : public Action
{
public:
    GreetAction(PlayerbotAI* botAI);
    bool Execute(Event event) override;
};

#endif
