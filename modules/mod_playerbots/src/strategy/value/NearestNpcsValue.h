#ifndef _PLAYERBOT_NEARESTNPCSVALUE_H
#define _PLAYERBOT_NEARESTNPCSVALUE_H

#include "NearestUnitsValue.h"
#include "PlayerbotAIConfig.h"

class PlayerbotAI;

class NearestNpcsValue : public NearestUnitsValue
{
public:
    NearestNpcsValue(PlayerbotAI* botAI, float range = sPlayerbotAIConfig->sightDistance)
        : NearestUnitsValue(botAI, "nearest npcs", range)
    {
    }

protected:
    void FindUnits(std::list<Unit*>& targets) override;
    bool AcceptUnit(Unit* unit) override;
};

#endif
