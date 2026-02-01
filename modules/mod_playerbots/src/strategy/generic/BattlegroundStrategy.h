/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_BATTLEGROUNDSTRATEGY_H
#define _PLAYERBOT_BATTLEGROUNDSTRATEGY_H

#include "PassTroughStrategy.h"

class BGStrategy : public PassTroughStrategy
{
public:
    BGStrategy(PlayerbotAI* botAI);

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "bg"; }
};

class BattlegroundStrategy : public PassTroughStrategy
{
public:
    BattlegroundStrategy(PlayerbotAI* botAI);

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "battleground"; }
};

class WarsongStrategy : public PassTroughStrategy
{
public:
    WarsongStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "warsong"; }
};

class AlteracStrategy : public PassTroughStrategy
{
public:
    AlteracStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "alterac"; }
};

class ArathiStrategy : public PassTroughStrategy
{
public:
    ArathiStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "arathi"; }
};

class EyeStrategy : public PassTroughStrategy
{
public:
    EyeStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "eye"; }
};

class IsleStrategy : public PassTroughStrategy
{
public:
    IsleStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "isle"; }
};

class ArenaStrategy : public PassTroughStrategy
{
public:
    ArenaStrategy(PlayerbotAI* botAI) : PassTroughStrategy(botAI) {}

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return "arena"; }
};

#endif
