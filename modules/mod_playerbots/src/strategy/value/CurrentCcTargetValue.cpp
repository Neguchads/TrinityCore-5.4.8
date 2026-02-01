#include "CurrentCcTargetValue.h"

#include "Playerbots.h"

class FindCurrentCcTargetStrategy : public FindTargetStrategy
{
public:
    FindCurrentCcTargetStrategy(PlayerbotAI* botAI, std::string const spell) : FindTargetStrategy(botAI), spell(spell)
    {
    }

    void CheckAttacker(Unit* attacker, ThreatManager* threatMgr) override
    {
        if (botAI->HasAura(spell, attacker))
            result = attacker;
    }

private:
    std::string const spell;
};

Unit* CurrentCcTargetValue::Calculate()
{
    FindCurrentCcTargetStrategy strategy(botAI, qualifier);
    return FindTarget(&strategy);
}
