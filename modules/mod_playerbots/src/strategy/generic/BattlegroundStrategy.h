/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
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
