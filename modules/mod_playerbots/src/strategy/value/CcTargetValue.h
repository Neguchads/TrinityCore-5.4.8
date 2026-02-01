#ifndef _PLAYERBOT_CCTARGETVALUE_H
#define _PLAYERBOT_CCTARGETVALUE_H

#include "NamedObjectContext.h"
#include "TargetValue.h"

class PlayerbotAI;
class Unit;

class CcTargetValue : public TargetValue, public Qualified
{
public:
    CcTargetValue(PlayerbotAI* botAI, std::string const name = "cc target") : TargetValue(botAI, name) {}

    Unit* Calculate() override;
};

#endif
