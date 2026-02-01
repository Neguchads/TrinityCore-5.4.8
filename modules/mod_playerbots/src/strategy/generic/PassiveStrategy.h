#ifndef _PLAYERBOT_PASSIVESTRATEGY_H
#define _PLAYERBOT_PASSIVESTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class PassiveStrategy : public Strategy
{
public:
    PassiveStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "passive"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

#endif
