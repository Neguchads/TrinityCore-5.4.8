#ifndef _PLAYERBOT_SHAMANSTRATEGY_H
#define _PLAYERBOT_SHAMANSTRATEGY_H

#include "GenericShamanStrategy.h"

class PlayerbotAI;

class ShamanStrategy : public GenericShamanStrategy
{
public:
    ShamanStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "shaman"; }
};

#endif
