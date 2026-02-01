#ifndef _PLAYERBOT_PROTECTIONPALADINSTRATEGY_H
#define _PLAYERBOT_PROTECTIONPALADINSTRATEGY_H

#include "TankPaladinStrategy.h"

class PlayerbotAI;

class ProtectionPaladinStrategy : public TankPaladinStrategy
{
public:
    ProtectionPaladinStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "protection"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
