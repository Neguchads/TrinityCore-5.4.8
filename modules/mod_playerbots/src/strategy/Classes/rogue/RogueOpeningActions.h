

#ifndef _PLAYERBOT_ROGUEOPENINGACTIONS_H
#define _PLAYERBOT_ROGUEOPENINGACTIONS_H

#include "GenericSpellActions.h"

class PlayerbotAI;
class Unit;

class CastSapAction : public CastMeleeSpellAction
{
public:
    CastSapAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "sap") {}

    Value<Unit*>* GetTargetValue() override;
    bool isUseful() override { return true; }
};

class CastGarroteAction : public CastDebuffSpellAction
{
public:
    CastGarroteAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "garrote", true, 8.0f) {}
};

class CastCheapShotAction : public CastMeleeSpellAction
{
public:
    CastCheapShotAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "cheap shot") {}
};

class CastAmbushAction : public CastMeleeSpellAction
{
public:
    CastAmbushAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "ambush") {}
};

#endif
