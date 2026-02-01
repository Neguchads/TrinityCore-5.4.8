#ifndef _PLAYERBOT_DPSFERALDRUIDSTRATEGY_H
#define _PLAYERBOT_DPSFERALDRUIDSTRATEGY_H

#include "CatDpsDruidStrategy.h"

class PlayerbotAI;

class DpsFeralDruidStrategy : public CatDpsDruidStrategy
{
public:
    DpsFeralDruidStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "dps feral"; }
};

#endif
