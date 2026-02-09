#ifndef _PLAYERBOT_CHECKMOUNTSTATEACTION_H
#define _PLAYERBOT_CHECKMOUNTSTATEACTION_H

#include "Action.h"

class CheckMountStateAction : public Action
{
public:
    CheckMountStateAction(PlayerbotAI* botAI) : Action(botAI, "check mount state") {}

    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override { return true; }

private:
    float CalculateDismountDistance() const;
    float CalculateMountDistance() const;
    bool ShouldFollowMasterMountState(Player* master, bool noAttackers, bool shouldMount) const;
    bool ShouldDismountForMaster(Player* master) const;
    bool Mount();
    void Dismount();
};

#endif
