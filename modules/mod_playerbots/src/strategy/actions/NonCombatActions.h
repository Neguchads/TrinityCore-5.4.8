#ifndef _PLAYERBOT_NONCOMBATACTIONS_H
#define _PLAYERBOT_NONCOMBATACTIONS_H

#include "UseItemAction.h"

class DrinkAction : public UseItemAction
{
public:
    DrinkAction(PlayerbotAI* botAI) : UseItemAction(botAI, "drink") {}

    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override;
};

class EatAction : public UseItemAction
{
public:
    EatAction(PlayerbotAI* botAI) : UseItemAction(botAI, "food") {}

    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override;
};

#endif
