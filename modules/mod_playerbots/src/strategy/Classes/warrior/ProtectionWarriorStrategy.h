#ifndef _PLAYERBOT_PROTECTIONWARRIORSTRATEGY_H
#define _PLAYERBOT_PROTECTIONWARRIORSTRATEGY_H

#include "TankWarriorStrategy.h"

class PlayerbotAI;

class ProtectionWarriorStrategy : public TankWarriorStrategy
{
public:
    ProtectionWarriorStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "protection"; }
};

#endif
