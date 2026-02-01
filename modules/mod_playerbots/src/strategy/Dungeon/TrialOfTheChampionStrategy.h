#ifndef _PLAYERBOT_TRIALOFTHECHAMPIONSTRATEGY_H
#define _PLAYERBOT_TRIALOFTHECHAMPIONSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class TrialOfTheChampionStrategy : public Strategy
{
public:
    TrialOfTheChampionStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "trial of the champion"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
