

#ifndef _PLAYERBOT_RELEASESPIRITACTION_H
#define _PLAYERBOT_RELEASESPIRITACTION_H

#include <cstdint>

#include "Action.h"
#include "ReviveFromCorpseAction.h"

class PlayerbotAI;

class ReleaseSpiritAction : public Action
{
public:
    ReleaseSpiritAction(PlayerbotAI* botAI, std::string const name = "release") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

class AutoReleaseSpiritAction : public ReleaseSpiritAction
{
public:
    AutoReleaseSpiritAction(PlayerbotAI* botAI, std::string const name = "auto release")
        : ReleaseSpiritAction(botAI, name)
    {
    }

    bool Execute(Event event) override;
    bool isUseful() override;

private:
    uint32_t bg_gossip_time = 0;
};

class RepopAction : public SpiritHealerAction
{
public:
    RepopAction(PlayerbotAI* botAI, std::string const name = "repop") : SpiritHealerAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

#endif
