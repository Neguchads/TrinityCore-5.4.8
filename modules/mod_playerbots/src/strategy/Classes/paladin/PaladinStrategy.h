#ifndef _PLAYERBOT_PALADINSTRATEGY_H
#define _PLAYERBOT_PALADINSTRATEGY_H

#include "GenericPaladinStrategy.h"

class PlayerbotAI;

class PaladinStrategy : public GenericPaladinStrategy
{
public:
    PaladinStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "paladin"; }
};

#endif
