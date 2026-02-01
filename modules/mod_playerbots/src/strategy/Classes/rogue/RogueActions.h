

#ifndef _PLAYERBOT_ROGUEACTIONS_H
#define _PLAYERBOT_ROGUEACTIONS_H

#include "GenericSpellActions.h"

class PlayerbotAI;

class CastEvasionAction : public CastBuffSpellAction
{
public:
    CastEvasionAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "evasion") {}
};

class CastCloakOfShadowsAction : public CastBuffSpellAction
{
public:
    CastCloakOfShadowsAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "cloak of shadows") {}
};

class CastHungerForBloodAction : public CastBuffSpellAction
{
public:
    CastHungerForBloodAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "hunger for blood") {}
    std::string const GetTargetName() override { return "current target"; }
};

class CastSprintAction : public CastBuffSpellAction
{
public:
    CastSprintAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "sprint") {}

    std::string const GetTargetName() override { return "self target"; }
};

class CastStealthAction : public CastBuffSpellAction
{
public:
    CastStealthAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "stealth") {}

    std::string const GetTargetName() override { return "self target"; }

    bool isPossible() override;
};

class UnstealthAction : public Action
{
public:
    UnstealthAction(PlayerbotAI* botAI) : Action(botAI, "unstealth") {}

    bool Execute(Event event) override;
};

class CheckStealthAction : public Action
{
public:
    CheckStealthAction(PlayerbotAI* botAI) : Action(botAI, "check stealth") {}

    bool isPossible() override { return true; }
    bool Execute(Event event) override;
};

class CastKickAction : public CastSpellAction
{
public:
    CastKickAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "kick") {}
};

class CastFeintAction : public CastBuffSpellAction
{
public:
    CastFeintAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "feint") {}
};

class CastDismantleAction : public CastSpellAction
{
public:
    CastDismantleAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "dismantle") {}
};

class CastDistractAction : public CastSpellAction
{
public:
    CastDistractAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "distract") {}
};

class CastVanishAction : public CastBuffSpellAction
{
public:
    CastVanishAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "vanish") {}

    bool isUseful() override;
};

class CastBlindAction : public CastDebuffSpellAction
{
public:
    CastBlindAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "blind") {}
};

class CastBladeFlurryAction : public CastBuffSpellAction
{
public:
    CastBladeFlurryAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "blade flurry") {}
};

class CastAdrenalineRushAction : public CastBuffSpellAction
{
public:
    CastAdrenalineRushAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "adrenaline rush") {}
};

class CastKillingSpreeAction : public CastMeleeSpellAction
{
public:
    CastKillingSpreeAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "killing spree") {}
};

class CastShadowBladesAction : public CastBuffSpellAction
{
public:
    CastShadowBladesAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "shadow blades") {}
};

class CastVendettaAction : public CastDebuffSpellAction
{
public:
    CastVendettaAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "vendetta") {}
};

class CastShadowDanceAction : public CastBuffSpellAction
{
public:
    CastShadowDanceAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "shadow dance") {}
};

class CastPreparationAction : public CastBuffSpellAction
{
public:
    CastPreparationAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "preparation") {}
};

class CastPremeditationAction : public CastSpellAction
{
public:
    CastPremeditationAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "premeditation") {}
};

class CastDispatchAction : public CastMeleeSpellAction
{
public:
    CastDispatchAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "dispatch") {}
};

class CastRevealingStrikeAction : public CastMeleeSpellAction
{
public:
    CastRevealingStrikeAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "revealing strike") {}
};

class CastHemorrhageAction : public CastMeleeSpellAction
{
public:
    CastHemorrhageAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "hemorrhage") {}
};

class CastShivAction : public CastMeleeSpellAction
{
public:
    CastShivAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "shiv") {}
};

class CastCrimsonTempestAction : public CastMeleeSpellAction
{
public:
    CastCrimsonTempestAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "crimson tempest") {}
};

class CastKickOnEnemyHealerAction : public CastSpellOnEnemyHealerAction
{
public:
    CastKickOnEnemyHealerAction(PlayerbotAI* botAI) : CastSpellOnEnemyHealerAction(botAI, "kick") {}
};

class EnvenomAction : public CastMeleeSpellAction
{
public:
    EnvenomAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "envenom") {}
};

class CastTricksOfTheTradeOnMainTankAction : public BuffOnMainTankAction
{
public:
    CastTricksOfTheTradeOnMainTankAction(PlayerbotAI* ai) : BuffOnMainTankAction(ai, "tricks of the trade", true) {}
    virtual bool isUseful() override;
};

class FanOfKnivesAction : public CastMeleeSpellAction
{
public:
    FanOfKnivesAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "fan of knives") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

#endif
