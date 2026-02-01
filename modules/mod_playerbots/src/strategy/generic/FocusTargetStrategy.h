#ifndef _PLAYERBOT_FOCUSTARGETSTRATEGY_H
#define _PLAYERBOT_FOCUSTARGETSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class FocusTargetStrategy : public Strategy
{
public:
    FocusTargetStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "focus heal targets"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class FocusRtiTargetsStrategy : public Strategy
{
public:
    FocusRtiTargetsStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "focus rti targets"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
