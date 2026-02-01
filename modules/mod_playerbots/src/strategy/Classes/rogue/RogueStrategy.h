#ifndef _PLAYERBOT_ROGUESTRATEGY_H
#define _PLAYERBOT_ROGUESTRATEGY_H

#include "DpsRogueStrategy.h"

class PlayerbotAI;

class RogueStrategy : public DpsRogueStrategy
{
public:
    RogueStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "rogue"; }
};

#endif
