#ifndef _PLAYERBOT_WARRIORSTRATEGY_H
#define _PLAYERBOT_WARRIORSTRATEGY_H

#include "GenericWarriorStrategy.h"

class PlayerbotAI;

class WarriorStrategy : public GenericWarriorStrategy
{
public:
    WarriorStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "warrior"; }
};

#endif
