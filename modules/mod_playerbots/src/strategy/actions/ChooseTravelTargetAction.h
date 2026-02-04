#ifndef _PLAYERBOT_CHOOSETRAVELTARGETACTION_H
#define _PLAYERBOT_CHOOSETRAVELTARGETACTION_H

#include "Action.h"

#include <vector>

class TravelTarget;
class TravelDestination;

class ChooseTravelTargetAction : public Action
{
public:
    ChooseTravelTargetAction(PlayerbotAI* botAI) : Action(botAI, "choose travel target") {}

    bool Execute(Event event) override;

private:
    void selectTarget(TravelTarget* newTarget, TravelTarget* oldTarget);
    bool SetTargetFromDestinations(TravelTarget* newTarget, std::vector<TravelDestination*> const& dests);
    void SetNullTarget(TravelTarget* newTarget);
    void setNewTarget(TravelTarget* newTarget, TravelTarget* oldTarget);
    void reportTarget(TravelTarget* newTarget, TravelTarget* oldTarget);
};

#endif
