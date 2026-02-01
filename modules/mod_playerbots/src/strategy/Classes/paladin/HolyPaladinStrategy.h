#ifndef _PLAYERBOT_HOLYPALADINSTRATEGY_H
#define _PLAYERBOT_HOLYPALADINSTRATEGY_H

#include "HealPaladinStrategy.h"

class PlayerbotAI;

class HolyPaladinStrategy : public HealPaladinStrategy
{
public:
    HolyPaladinStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "holy"; }
};

#endif
