#ifndef _PLAYERBOT_PRIESTSTRATEGY_H
#define _PLAYERBOT_PRIESTSTRATEGY_H

#include "GenericPriestStrategy.h"

class PlayerbotAI;

class PriestStrategy : public GenericPriestStrategy
{
public:
    PriestStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "priest"; }
};

#endif
