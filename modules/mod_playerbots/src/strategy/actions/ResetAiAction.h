#ifndef _PLAYERBOT_RESETAIACTION_H
#define _PLAYERBOT_RESETAIACTION_H

#include "Action.h"
#include "NamedObjectContext.h"

class ResetAiAction : public Action
{
public:
    ResetAiAction(PlayerbotAI* botAI, bool fullReset = true, std::string const& name = "reset ai")
        : Action(botAI, name), fullReset(fullReset)
    {
    }

    bool Execute(Event event) override;
    bool isUsefulWhenStunned() override { return true; }

protected:
    void ResetStrategies();
    void ResetValues();

    bool fullReset;
};

class ResetStratsAction : public ResetAiAction
{
public:
    ResetStratsAction(PlayerbotAI* botAI, std::string const& name = "reset strats", bool fullReset = true)
        : ResetAiAction(botAI, fullReset, name)
    {
    }

    bool Execute(Event event) override;
};

class ResetValuesAction : public ResetAiAction
{
public:
    ResetValuesAction(PlayerbotAI* botAI, std::string const& name = "reset values", bool fullReset = true)
        : ResetAiAction(botAI, fullReset, name)
    {
    }

    bool Execute(Event event) override;
};

class SaveAiAction : public Action, public Qualified
{
public:
    SaveAiAction(PlayerbotAI* botAI, std::string const& name = "save ai") : Action(botAI, name), Qualified() {}

    bool Execute(Event event) override;
    bool isUseful() override { return true; }
    bool isUsefulWhenStunned() override { return true; }
};

class LoadAiAction : public Action, public Qualified
{
public:
    LoadAiAction(PlayerbotAI* botAI, std::string const& name = "load ai") : Action(botAI, name), Qualified() {}

    bool Execute(Event event) override;
    bool isUseful() override { return true; }
    bool isUsefulWhenStunned() override { return true; }
};

class ListAiAction : public Action, public Qualified
{
public:
    ListAiAction(PlayerbotAI* botAI, std::string const& name = "list ai") : Action(botAI, name), Qualified() {}

    bool Execute(Event event) override;
    bool isUseful() override { return true; }
    bool isUsefulWhenStunned() override { return true; }
};

class ResetAction : public Action
{
public:
    ResetAction(PlayerbotAI* botAI, std::string const& name = "reset") : Action(botAI, name) {}

    bool Execute(Event event) override;

    bool isUsefulWhenStunned() override { return true; }
};

#endif
