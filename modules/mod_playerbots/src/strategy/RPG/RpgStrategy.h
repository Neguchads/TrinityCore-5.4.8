#ifndef _PLAYERBOT_RPGSTRATEGY_H
#define _PLAYERBOT_RPGSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

class RpgActionMultiplier : public Multiplier
{
public:
    RpgActionMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "rpg action") {}

    float GetValue(Action* action) override;
};

class RpgStrategy : public Strategy
{
public:
    RpgStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "rpg"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

class RpgQuestStrategy : public Strategy
{
public:
    RpgQuestStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "rpg quest"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RpgVendorStrategy : public Strategy
{
public:
    RpgVendorStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "rpg vendor"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RpgExploreStrategy : public Strategy
{
public:
    RpgExploreStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "rpg explore"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RpgMaintenanceStrategy : public Strategy
{
public:
    RpgMaintenanceStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "rpg maintenance"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RpgGuildStrategy : public Strategy
{
public:
    RpgGuildStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "rpg guild"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RpgBgStrategy : public Strategy
{
public:
    RpgBgStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "rpg bg"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RpgPlayerStrategy : public Strategy
{
public:
    RpgPlayerStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "rpg player"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RpgCraftStrategy : public Strategy
{
public:
    RpgCraftStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "rpg craft"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class RpgJumpStrategy : public Strategy
{
public:
    RpgJumpStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "rpg jump"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
