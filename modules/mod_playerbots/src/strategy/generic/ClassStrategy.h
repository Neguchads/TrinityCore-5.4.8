#ifndef _PLAYERBOT_CLASSSTRATEGY_H
#define _PLAYERBOT_CLASSSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class ClassStrategy : public Strategy
{
public:
    ClassStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "class"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
