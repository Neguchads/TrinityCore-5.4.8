#ifndef _PLAYERBOT_WARLOCKSTRATEGY_H
#define _PLAYERBOT_WARLOCKSTRATEGY_H

#include "GenericWarlockStrategy.h"

class PlayerbotAI;

class WarlockStrategy : public GenericWarlockStrategy
{
public:
    WarlockStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "warlock"; }
};

#endif
