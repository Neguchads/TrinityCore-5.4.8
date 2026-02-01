#ifndef _PLAYERBOT_DRUIDSTRATEGY_H
#define _PLAYERBOT_DRUIDSTRATEGY_H

#include "GenericDruidStrategy.h"

class PlayerbotAI;

class DruidStrategy : public GenericDruidStrategy
{
public:
    DruidStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "druid"; }
};

#endif
