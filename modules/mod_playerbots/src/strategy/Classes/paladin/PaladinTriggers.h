

#ifndef _PLAYERBOT_PALADINTRIGGERS_H
#define _PLAYERBOT_PALADINTRIGGERS_H

#include "GenericTriggers.h"
#include "SharedDefines.h"
#include "Unit.h"

class PlayerbotAI;

class SealTrigger : public HasAuraTrigger
{
public:
	SealTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "seal", 5.0f) {}
	bool IsActive() override;
};

class HolyPowerTrigger : public Trigger
{
public:
    HolyPowerTrigger(PlayerbotAI* botAI, uint8 minHolyPower) : Trigger(botAI, "holy power"), minHolyPower(minHolyPower) {}
    bool IsActive() override;

private:
    uint8 minHolyPower;
};

class ArtOfWarTrigger : public HasAuraTrigger
{
public:
    ArtOfWarTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "the art of war") {}
};

class GrandCrusaderTrigger : public HasAuraTrigger
{
public:
    GrandCrusaderTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "grand crusader") {}
};

class DivinePurposeTrigger : public HasAuraTrigger
{
public:
    DivinePurposeTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "divine purpose") {}
};

class InquisitionTrigger : public HasNoAuraTrigger
{
public:
    InquisitionTrigger(PlayerbotAI* botAI) : HasNoAuraTrigger(botAI, "inquisition") {}
};

class BastionOfGloryTrigger : public HasAuraStackTrigger
{
public:
    BastionOfGloryTrigger(PlayerbotAI* botAI, uint8 minStacks)
        : HasAuraStackTrigger(botAI, "bastion of glory", minStacks, 1)
    {
    }
};

class AvengingWrathTrigger : public BoostTrigger
{
public:
    AvengingWrathTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "avenging wrath") {}
};

class HolyAvengerTrigger : public BoostTrigger
{
public:
    HolyAvengerTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "holy avenger") {}
};

class GuardianOfAncientKingsTrigger : public BoostTrigger
{
public:
    GuardianOfAncientKingsTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "guardian of ancient kings") {}
};

#endif /*_PLAYERBOT_PALADINTRIGGERS_H*/
