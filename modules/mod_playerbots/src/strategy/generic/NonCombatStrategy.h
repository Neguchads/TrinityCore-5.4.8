#ifndef _PLAYERBOT_NONCOMBATSTRATEGY_H
#define _PLAYERBOT_NONCOMBATSTRATEGY_H

#include "PassTroughStrategy.h"

class PlayerbotAI;
class NonCombatStrategy : public Strategy
{
public:
    NonCombatStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class CollisionStrategy : public Strategy
{
public:
    CollisionStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    std::string const getName() override { return "collision"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class MountStrategy : public Strategy
{
public:
    MountStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    std::string const getName() override { return "mount"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class AttackTaggedStrategy : public Strategy
{
public:
    AttackTaggedStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    std::string const getName() override { return "attack tagged"; }
};

class WorldBuffStrategy : public Strategy
{
public:
    WorldBuffStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    std::string const getName() override { return "worldbuff"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class SilentStrategy : public Strategy
{
public:
    SilentStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    std::string const getName() override { return "silent"; }
};

class NoWarStrategy : public Strategy
{
public:
    NoWarStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    std::string const getName() override { return "nowar"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class GlyphStrategy : public Strategy
{
public:
    GlyphStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    std::string const getName() override { return "glyph"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class FishStrategy : public Strategy
{
public:
    FishStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    std::string const getName() override { return "fish"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;

protected:
    std::string modifier;
};

class TFishStrategy : public FishStrategy
{
public:
    TFishStrategy(PlayerbotAI* botAI) : FishStrategy(botAI) { modifier = "::travel"; }

    std::string const getName() override { return "tfish"; }
};

class MasterFishingStrategy : public Strategy
{
public:
    MasterFishingStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    std::string const getName() override { return "master fishing"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
