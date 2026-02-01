#ifndef _PLAYERBOT_SUBTLETYROGUESTRATEGY_H
#define _PLAYERBOT_SUBTLETYROGUESTRATEGY_H

#include "DpsRogueStrategy.h"

class PlayerbotAI;

class SubtletyRogueStrategy : public DpsRogueStrategy
{
public:
    SubtletyRogueStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "subtlety"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
