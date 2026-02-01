/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_WINDWALKERMONKSTRATEGY_H
#define _PLAYERBOT_WINDWALKERMONKSTRATEGY_H

#include "GenericMonkStrategy.h"

class PlayerbotAI;

class WindwalkerMonkStrategy : public GenericMonkStrategy
{
public:
    WindwalkerMonkStrategy(PlayerbotAI* botAI);

    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "melee"; }
};

#endif
