#ifndef _PLAYERBOT_MAGESTRATEGY_H
#define _PLAYERBOT_MAGESTRATEGY_H

#include "GenericMageStrategy.h"

class PlayerbotAI;

class MageStrategy : public GenericMageStrategy
{
public:
    MageStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "mage"; }
};

#endif
