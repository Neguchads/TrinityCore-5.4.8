

#include "NearestUnitsValue.h"

#include "Playerbots.h"

GuidVector NearestUnitsValue::Calculate()
{
    std::list<Unit*> targets;
    FindUnits(targets);

    GuidVector results;
    for (Unit* unit : targets)
    {
        if (AcceptUnit(unit) && (ignoreLos || bot->IsWithinLOSInMap(unit)))
            results.push_back(unit->GetGUID());
    }

    return results;
}
