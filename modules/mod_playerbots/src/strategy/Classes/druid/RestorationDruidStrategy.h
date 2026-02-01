#ifndef _PLAYERBOT_RESTORATIONDRUIDSTRATEGY_H
#define _PLAYERBOT_RESTORATIONDRUIDSTRATEGY_H

#include "HealDruidStrategy.h"

class PlayerbotAI;

class RestorationDruidStrategy : public HealDruidStrategy
{
public:
    RestorationDruidStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "restoration"; }
};

#endif
