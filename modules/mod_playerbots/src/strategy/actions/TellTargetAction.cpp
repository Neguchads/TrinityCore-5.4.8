#include "TellTargetAction.h"

#include <sstream>

#include "Event.h"
#include "Playerbots.h"
#include "HostileRefManager.h"
#include "ThreatManager.h"

bool TellTargetAction::Execute(Event event)
{
    Unit* target = context->GetValue<Unit*>("current target")->Get();
    if (target)
    {
        std::ostringstream out;
        out << "Attacking " << target->GetName();
        botAI->TellMaster(out);

        context->GetValue<Unit*>("old target")->Set(target);
    }

    return true;
}

bool TellAttackersAction::Execute(Event event)
{
    botAI->TellMaster("--- Attackers ---");

    GuidVector attackers = context->GetValue<GuidVector>("attackers")->Get();
    int32 count = 0;
    for (ObjectGuid const guid : attackers)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        botAI->TellMaster(std::to_string(++count) + std::string(".") + unit->GetName());
    }

    botAI->TellMaster("--- Threat ---");

    HostileReference* ref = bot->getHostileRefManager().getFirst();
    if (!ref)
        return true;

    while (ref)
    {
        ThreatManager* threatMgr = ref->GetSource();
        Unit* unit = threatMgr ? threatMgr->GetOwner() : nullptr;
        float threat = ref->getThreat();

        if (unit)
        {
            std::ostringstream out;
            out << unit->GetName() << " (" << threat << ")";
            botAI->TellMaster(out);
        }

        ref = ref->next();
    }

    return true;
}
