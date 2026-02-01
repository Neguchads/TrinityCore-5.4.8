
#ifndef _PLAYERBOT_DKTRIGGERS_H
#define _PLAYERBOT_DKTRIGGERS_H

#include "GenericTriggers.h"

class PlayerbotAI;

class FreezingFogTrigger : public HasAuraTrigger
{
public:
    FreezingFogTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "freezing fog") {}
};

class KillingMachineTrigger : public HasAuraTrigger
{
public:
    KillingMachineTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "killing machine") {}
};

class CrimsonScourgeTrigger : public HasAuraTrigger
{
public:
    CrimsonScourgeTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "crimson scourge") {}
};

class SuddenDoomTrigger : public HasAuraTrigger
{
public:
    SuddenDoomTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "sudden doom") {}
};

class DarkTransformationTrigger : public HasAuraTrigger
{
public:
    DarkTransformationTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "shadow infusion") {}
};

class ScentOfBloodTrigger : public HasAuraTrigger
{
public:
    ScentOfBloodTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "scent of blood") {}
};

#endif
