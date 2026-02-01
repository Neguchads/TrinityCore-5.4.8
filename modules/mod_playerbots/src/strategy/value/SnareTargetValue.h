#ifndef _PLAYERBOT_SNARETARGETVALUE_H
#define _PLAYERBOT_SNARETARGETVALUE_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;
class Unit;

class SnareTargetValue : public UnitCalculatedValue, public Qualified
{
public:
    SnareTargetValue(PlayerbotAI* botAI) : UnitCalculatedValue(botAI, "snare target", 1) {}

protected:
    Unit* Calculate() override;
};

#endif
