#ifndef _PLAYERBOT_AFFLICTIONWARLOCKSTRATEGY_H
#define _PLAYERBOT_AFFLICTIONWARLOCKSTRATEGY_H

#include "AffliWarlockStrategy.h"

class PlayerbotAI;

class AfflictionWarlockStrategy : public AffliWarlockStrategy
{
public:
    AfflictionWarlockStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "affli"; }
};

#endif
