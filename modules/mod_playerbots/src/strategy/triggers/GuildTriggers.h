#ifndef _PLAYERBOT_GUILDTRIGGERS_H
#define _PLAYERBOT_GUILDTRIGGERS_H

#include "Trigger.h"
#include "Playerbots.h"

class PlayerbotAI;

class LeaveLargeGuildTrigger : public Trigger
{
public:
    LeaveLargeGuildTrigger(PlayerbotAI* botAI) : Trigger(botAI, "leave large guild", 10) {}
    bool IsActive() override;
};

class PetitionTurnInTrigger : public Trigger
{
public:
    PetitionTurnInTrigger(PlayerbotAI* botAI) : Trigger(botAI, "petition signed", 5) {}
    bool IsActive() override { return AI_VALUE(bool, "can hand in petition"); }
};

class BuyTabardTrigger : public Trigger
{
public:
    BuyTabardTrigger(PlayerbotAI* botAI) : Trigger(botAI, "buy tabard", 5) {}
    bool IsActive() override { return AI_VALUE(bool, "can buy tabard"); }
};

#endif
