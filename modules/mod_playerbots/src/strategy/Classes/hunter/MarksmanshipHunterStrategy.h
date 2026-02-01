#ifndef _PLAYERBOT_MARKSMANSHIPHUNTERSTRATEGY_H
#define _PLAYERBOT_MARKSMANSHIPHUNTERSTRATEGY_H

#include "DpsHunterStrategy.h"

class PlayerbotAI;

class MarksmanshipHunterStrategy : public DpsHunterStrategy
{
public:
    MarksmanshipHunterStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "mm"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
