#ifndef _PLAYERBOT_DEBUGSTRATEGY_H
#define _PLAYERBOT_DEBUGSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class DebugStrategy : public Strategy
{
public:
    DebugStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "debug"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class DebugActionStrategy : public DebugStrategy
{
public:
    DebugActionStrategy(PlayerbotAI* botAI) : DebugStrategy(botAI) {}

    std::string const getName() override { return "debug action"; }
};

class DebugMoveStrategy : public Strategy
{
public:
    DebugMoveStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug move"; }
};

class DebugRpgStrategy : public Strategy
{
public:
    DebugRpgStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug rpg"; }
};

class DebugSpellStrategy : public Strategy
{
public:
    DebugSpellStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug spell"; }
};

class DebugTravelStrategy : public Strategy
{
public:
    DebugTravelStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug travel"; }
};

class DebugThreatStrategy : public Strategy
{
public:
    DebugThreatStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug threat"; }
};

class DebugMountStrategy : public Strategy
{
public:
    DebugMountStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug mount"; }
};

class DebugGrindStrategy : public Strategy
{
public:
    DebugGrindStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug grind"; }
};

class DebugLootStrategy : public Strategy
{
public:
    DebugLootStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug loot"; }
};

class DebugLogStrategy : public Strategy
{
public:
    DebugLogStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug log"; }
};

class DebugLogNameStrategy : public Strategy
{
public:
    DebugLogNameStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug logname"; }
};

class DebugQuestStrategy : public Strategy
{
public:
    DebugQuestStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug quest"; }
};

class AIChatStrategy : public Strategy
{
public:
    AIChatStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "ai chat"; }
};

class DebugLLMStrategy : public Strategy
{
public:
    DebugLLMStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug llm"; }
};

class DebugStuckStrategy : public Strategy
{
public:
    DebugStuckStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug stuck"; }
};

class DebugXpStrategy : public Strategy
{
public:
    DebugXpStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "debug xp"; }
};

#endif
