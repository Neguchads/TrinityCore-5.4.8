

#ifndef _PLAYERBOT_TARGETVALUE_H
#define _PLAYERBOT_TARGETVALUE_H

#include "NamedObjectContext.h"
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

#endif
