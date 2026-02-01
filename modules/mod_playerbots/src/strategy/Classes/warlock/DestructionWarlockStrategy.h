#ifndef _PLAYERBOT_DESTRUCTIONWARLOCKSTRATEGY_H
#define _PLAYERBOT_DESTRUCTIONWARLOCKSTRATEGY_H

#include "DestruWarlockStrategy.h"

class PlayerbotAI;

class DestructionWarlockStrategy : public DestruWarlockStrategy
{
public:
    DestructionWarlockStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "destro"; }
};

#endif
