#ifndef _PLAYERBOT_FROSTFIREMAGESTRATEGY_H
#define _PLAYERBOT_FROSTFIREMAGESTRATEGY_H

#include "GenericMageStrategy.h"

class PlayerbotAI;

class FrostFireMageStrategy : public GenericMageStrategy
{
public:
    FrostFireMageStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "frostfire"; }
};

#endif
