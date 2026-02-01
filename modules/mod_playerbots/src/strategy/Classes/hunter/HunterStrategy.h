#ifndef _PLAYERBOT_HUNTERSTRATEGY_H
#define _PLAYERBOT_HUNTERSTRATEGY_H

#include "GenericHunterStrategy.h"

class PlayerbotAI;

class HunterStrategy : public GenericHunterStrategy
{
public:
    HunterStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "hunter"; }
};

#endif
