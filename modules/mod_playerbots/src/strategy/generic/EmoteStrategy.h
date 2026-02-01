#ifndef _PLAYERBOT_EMOTESTRATEGY_H
#define _PLAYERBOT_EMOTESTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class EmoteStrategy : public Strategy
{
public:
    EmoteStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "emote"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
