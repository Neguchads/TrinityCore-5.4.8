

#ifndef _PLAYERBOT_TARGETVALUE_H
#define _PLAYERBOT_TARGETVALUE_H

#include "NamedObjectContext.h"
#include "ObjectGuid.h"
#include "TravelMgr.h"
#include "Value.h"

class PlayerbotAI;
class ThreatManager;
class Unit;

class FindTargetStrategy
{
public:
    FindTargetStrategy(PlayerbotAI* botAI) : result(nullptr), botAI(botAI) {}

    Unit* GetResult();
    virtual void CheckAttacker(Unit* attacker, ThreatManager* threatMgr) = 0;
    void GetPlayerCount(Unit* creature, uint32* tankCount, uint32* dpsCount);
    bool IsHighPriority(Unit* attacker);

protected:
    Unit* result;
    PlayerbotAI* botAI;
    std::map<Unit*, uint32> tankCountCache;
    std::map<Unit*, uint32> dpsCountCache;
    bool foundHighPriority = false;
};

class FindNonCcTargetStrategy : public FindTargetStrategy
{
public:
    FindNonCcTargetStrategy(PlayerbotAI* botAI) : FindTargetStrategy(botAI) {}

protected:
    virtual bool IsCcTarget(Unit* attacker);
};

class TargetValue : public UnitCalculatedValue
{
public:
    TargetValue(PlayerbotAI* botAI, std::string const name = "target", int checkInterval = 1)
        : UnitCalculatedValue(botAI, name, checkInterval)
    {
    }

protected:
    Unit* FindTarget(FindTargetStrategy* strategy);
};

class PullTargetValue : public ManualSetValue<ObjectGuid>
{
public:
    PullTargetValue(PlayerbotAI* botAI, std::string const name = "pull target")
        : ManualSetValue<ObjectGuid>(botAI, ObjectGuid::Empty, name)
    {
    }
};

class RpgTargetValue : public ManualSetValue<GuidPosition>
{
public:
    RpgTargetValue(PlayerbotAI* botAI, std::string const name = "rpg target")
        : ManualSetValue<GuidPosition>(botAI, GuidPosition(), name)
    {
    }
};

class TravelTargetValue : public ManualSetValue<TravelTarget*>
{
public:
    TravelTargetValue(PlayerbotAI* botAI, std::string const name = "travel target")
        : ManualSetValue<TravelTarget*>(botAI, new TravelTarget(botAI), name)
    {
    }

    ~TravelTargetValue() override { delete value; }
};

class LastLongMoveValue : public CalculatedValue<WorldPosition>
{
public:
    LastLongMoveValue(PlayerbotAI* botAI) : CalculatedValue<WorldPosition>(botAI, "last long move", 30 * 1000) {}

    WorldPosition Calculate() override;
};

class HomeBindValue : public CalculatedValue<WorldPosition>
{
public:
    HomeBindValue(PlayerbotAI* botAI) : CalculatedValue<WorldPosition>(botAI, "home bind", 30 * 1000) {}

    WorldPosition Calculate() override;
};

class IgnoreRpgTargetValue : public ManualSetValue<GuidSet&>
{
public:
    IgnoreRpgTargetValue(PlayerbotAI* botAI) : ManualSetValue<GuidSet&>(botAI, data, "ignore rpg target") {}

private:
    GuidSet data;
};

#endif
