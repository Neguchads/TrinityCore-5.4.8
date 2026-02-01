#ifndef _PLAYERBOT_OFFHEALDRUIDCATSTRATEGY_H
#define _PLAYERBOT_OFFHEALDRUIDCATSTRATEGY_H

#include "CatDpsDruidStrategy.h"

class PlayerbotAI;

class OffhealDruidCatStrategy : public CatDpsDruidStrategy
{
public:
    OffhealDruidCatStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "offheal"; }
};

#endif
