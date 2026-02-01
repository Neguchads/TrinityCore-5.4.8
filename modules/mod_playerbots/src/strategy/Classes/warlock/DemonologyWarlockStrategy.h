#ifndef _PLAYERBOT_DEMONOLOGYWARLOCKSTRATEGY_H
#define _PLAYERBOT_DEMONOLOGYWARLOCKSTRATEGY_H

#include "DemonoWarlockStrategy.h"

class PlayerbotAI;

class DemonologyWarlockStrategy : public DemonoWarlockStrategy
{
public:
    DemonologyWarlockStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "demo"; }
};

#endif
