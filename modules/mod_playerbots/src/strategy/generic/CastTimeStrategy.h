#ifndef _PLAYERBOT_CASTTIMESTRATEGY_H
#define _PLAYERBOT_CASTTIMESTRATEGY_H

#include "Multiplier.h"
#include "Strategy.h"

class Action;
class PlayerbotAI;

class CastTimeMultiplier : public Multiplier
{
public:
    CastTimeMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "cast time") {}

    float GetValue(Action* action) override;
};

class CastTimeStrategy : public Strategy
{
public:
    CastTimeStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "cast time"; }
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

#endif
