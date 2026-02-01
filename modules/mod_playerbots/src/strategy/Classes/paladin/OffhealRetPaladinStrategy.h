#ifndef _PLAYERBOT_OFFHEALRETPALADINSTRATEGY_H
#define _PLAYERBOT_OFFHEALRETPALADINSTRATEGY_H

#include "DpsPaladinStrategy.h"

class PlayerbotAI;

class OffhealRetPaladinStrategy : public DpsPaladinStrategy
{
public:
    OffhealRetPaladinStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "offheal"; }
};

#endif
