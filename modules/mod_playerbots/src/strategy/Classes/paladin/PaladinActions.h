

#ifndef _PLAYERBOT_PALADINACTIONS_H
#define _PLAYERBOT_PALADINACTIONS_H

#include "AiObject.h"
#include "GenericSpellActions.h"
#include "SharedDefines.h"

class PlayerbotAI;
class Unit;

// seals
BUFF_ACTION(CastSealOfRighteousnessAction, "seal of righteousness");
BUFF_ACTION(CastSealOfCommandAction, "seal of command");
BUFF_ACTION(CastSealOfInsightAction, "seal of insight");
BUFF_ACTION(CastSealOfTruthAction, "seal of truth");

// judgements
SPELL_ACTION(CastJudgementAction, "judgement");

// auras
BUFF_ACTION(CastDevotionAuraAction, "devotion aura");
BUFF_ACTION(CastRetributionAuraAction, "retribution aura");
BUFF_ACTION(CastConcentrationAuraAction, "concentration aura");
BUFF_ACTION(CastCrusaderAuraAction, "crusader aura");
BUFF_ACTION(CastSanctityAuraAction, "sanctity aura");

SPELL_ACTION(CastHolyShockAction, "holy shock");

// consecration
MELEE_ACTION(CastConsecrationAction, "consecration");

// repentance
SNARE_ACTION(CastRepentanceSnareAction, "repentance");
DEBUFF_ACTION(CastRepentanceAction, "repentance");
ENEMY_HEALER_ACTION(CastRepentanceOnHealerAction, "repentance");

// hamme of wrath
SPELL_ACTION(CastHammerOfWrathAction, "hammer of wrath");

// buffs
BUFF_ACTION(CastDivineFavorAction, "divine favor");

// blessings

// fury
BUFF_ACTION(CastRighteousFuryAction, "righteous fury");

class CastDivineStormAction : public CastMeleeSpellAction
{
public:
    CastDivineStormAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "divine storm") {}
};

class CastCrusaderStrikeAction : public CastMeleeSpellAction
{
public:
    CastCrusaderStrikeAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "crusader strike") {}
    bool isUseful() override;
};

class CastSealSpellAction : public CastBuffSpellAction
{
public:
    CastSealSpellAction(PlayerbotAI* botAI, std::string const name) : CastBuffSpellAction(botAI, name) {}

    bool isUseful() override;
};

class CastBlessingOfMightAction : public CastBuffSpellAction
{
public:
    CastBlessingOfMightAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "blessing of might") {}
};

class CastBlessingOnPartyAction : public BuffOnPartyAction
{
public:
    CastBlessingOnPartyAction(PlayerbotAI* botAI, std::string const name) : BuffOnPartyAction(botAI, name), name(name)
    {
    }

private:
    std::string name;
};

class CastBlessingOfMightOnPartyAction : public BuffOnPartyAction
{
public:
    CastBlessingOfMightOnPartyAction(PlayerbotAI* botAI) : BuffOnPartyAction(botAI, "blessing of might") {}

    std::string const getName() override { return "blessing of might on party"; }
};

class CastBlessingOfKingsAction : public CastBuffSpellAction
{
public:
    CastBlessingOfKingsAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "blessing of kings") {}
};

class CastBlessingOfKingsOnPartyAction : public CastBlessingOnPartyAction
{
public:
    CastBlessingOfKingsOnPartyAction(PlayerbotAI* botAI) : CastBlessingOnPartyAction(botAI, "blessing of kings") {}

    std::string const getName() override { return "blessing of kings on party"; }
};

class CastHolyLightAction : public CastHealingSpellAction
{
public:
    CastHolyLightAction(PlayerbotAI* botAI) : CastHealingSpellAction(botAI, "holy light") {}
};

class CastHolyShockOnPartyAction : public HealPartyMemberAction
{
public:
    CastHolyShockOnPartyAction(PlayerbotAI* botAI)
        : HealPartyMemberAction(botAI, "holy shock", 25.0f, HealingManaEfficiency::LOW)
    {
    }
};

class CastHolyLightOnPartyAction : public HealPartyMemberAction
{
public:
    CastHolyLightOnPartyAction(PlayerbotAI* botAI)
        : HealPartyMemberAction(botAI, "holy light", 50.0f, HealingManaEfficiency::MEDIUM)
    {
    }
};

class CastFlashOfLightAction : public CastHealingSpellAction
{
public:
    CastFlashOfLightAction(PlayerbotAI* botAI) : CastHealingSpellAction(botAI, "flash of light") {}
};

class CastFlashOfLightOnPartyAction : public HealPartyMemberAction
{
public:
    CastFlashOfLightOnPartyAction(PlayerbotAI* botAI)
        : HealPartyMemberAction(botAI, "flash of light", 15.0f, HealingManaEfficiency::HIGH)
    {
    }
};

class CastDivineLightAction : public CastHealingSpellAction
{
public:
    CastDivineLightAction(PlayerbotAI* botAI) : CastHealingSpellAction(botAI, "divine light") {}
};

class CastDivineLightOnPartyAction : public HealPartyMemberAction
{
public:
    CastDivineLightOnPartyAction(PlayerbotAI* botAI)
        : HealPartyMemberAction(botAI, "divine light", 50.0f, HealingManaEfficiency::LOW)
    {
    }
};

class CastHolyRadianceAction : public CastHealingSpellAction
{
public:
    CastHolyRadianceAction(PlayerbotAI* botAI) : CastHealingSpellAction(botAI, "holy radiance") {}
};

class CastHolyRadianceOnPartyAction : public HealPartyMemberAction
{
public:
    CastHolyRadianceOnPartyAction(PlayerbotAI* botAI)
        : HealPartyMemberAction(botAI, "holy radiance", 20.0f, HealingManaEfficiency::LOW)
    {
    }
};

class CastLightOfDawnAction : public CastHealingSpellAction
{
public:
    CastLightOfDawnAction(PlayerbotAI* botAI) : CastHealingSpellAction(botAI, "light of dawn") {}
    bool isUseful() override;
};

class CastLightOfDawnOnPartyAction : public HealPartyMemberAction
{
public:
    CastLightOfDawnOnPartyAction(PlayerbotAI* botAI)
        : HealPartyMemberAction(botAI, "light of dawn", 30.0f, HealingManaEfficiency::HIGH)
    {
    }
    bool isUseful() override;
};

class CastWordOfGloryAction : public CastHealingSpellAction
{
public:
    CastWordOfGloryAction(PlayerbotAI* botAI) : CastHealingSpellAction(botAI, "word of glory") {}
    bool isUseful() override;
};

class CastWordOfGloryOnPartyAction : public HealPartyMemberAction
{
public:
    CastWordOfGloryOnPartyAction(PlayerbotAI* botAI)
        : HealPartyMemberAction(botAI, "word of glory", 30.0f, HealingManaEfficiency::HIGH)
    {
    }
    bool isUseful() override;
};

class CastEternalFlameAction : public CastHealingSpellAction
{
public:
    CastEternalFlameAction(PlayerbotAI* botAI) : CastHealingSpellAction(botAI, "eternal flame") {}
    bool isUseful() override;
};

class CastEternalFlameOnPartyAction : public HealPartyMemberAction
{
public:
    CastEternalFlameOnPartyAction(PlayerbotAI* botAI)
        : HealPartyMemberAction(botAI, "eternal flame", 30.0f, HealingManaEfficiency::HIGH)
    {
    }
    bool isUseful() override;
};

class CastLayOnHandsAction : public CastHealingSpellAction
{
public:
    CastLayOnHandsAction(PlayerbotAI* botAI) : CastHealingSpellAction(botAI, "lay on hands") {}
};

class CastLayOnHandsOnPartyAction : public HealPartyMemberAction
{
public:
    CastLayOnHandsOnPartyAction(PlayerbotAI* botAI) : HealPartyMemberAction(botAI, "lay on hands") {}
};

class CastDivineProtectionAction : public CastBuffSpellAction
{
public:
    CastDivineProtectionAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "divine protection") {}
};

class CastDivineProtectionOnPartyAction : public HealPartyMemberAction
{
public:
    CastDivineProtectionOnPartyAction(PlayerbotAI* botAI) : HealPartyMemberAction(botAI, "divine protection") {}

    std::string const getName() override { return "divine protection on party"; }
};

class CastDivineShieldAction : public CastBuffSpellAction
{
public:
    CastDivineShieldAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "divine shield") {}
};

class CastArdentDefenderAction : public CastBuffSpellAction
{
public:
    CastArdentDefenderAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "ardent defender") {}
};

class CastHolyWrathAction : public CastMeleeSpellAction
{
public:
    CastHolyWrathAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "holy wrath") {}
};

class CastHammerOfJusticeAction : public CastMeleeSpellAction
{
public:
    CastHammerOfJusticeAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "hammer of justice") {}
};

class CastHammerOfTheRighteousAction : public CastMeleeSpellAction
{
public:
    CastHammerOfTheRighteousAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "hammer of the righteous") {}
};

class CastPurifyPoisonAction : public CastCureSpellAction
{
public:
    CastPurifyPoisonAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "purify") {}
};

class CastPurifyDiseaseAction : public CastCureSpellAction
{
public:
    CastPurifyDiseaseAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "purify") {}
};

class CastPurifyPoisonOnPartyAction : public CurePartyMemberAction
{
public:
    CastPurifyPoisonOnPartyAction(PlayerbotAI* botAI) : CurePartyMemberAction(botAI, "purify", DISPEL_POISON) {}

    std::string const getName() override { return "purify poison on party"; }
};

class CastPurifyDiseaseOnPartyAction : public CurePartyMemberAction
{
public:
    CastPurifyDiseaseOnPartyAction(PlayerbotAI* botAI) : CurePartyMemberAction(botAI, "purify", DISPEL_DISEASE) {}

    std::string const getName() override { return "purify disease on party"; }
};

class CastHandOfReckoningAction : public CastSpellAction
{
public:
    CastHandOfReckoningAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "hand of reckoning") {}
};

class CastRighteousDefenseAction : public CastSpellAction
{
public:
    CastRighteousDefenseAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "righteous defense") {}
    virtual Unit* GetTarget() override;
};

class CastCleansePoisonAction : public CastCureSpellAction
{
public:
    CastCleansePoisonAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "cleanse") {}
};

class CastCleanseDiseaseAction : public CastCureSpellAction
{
public:
    CastCleanseDiseaseAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "cleanse") {}
};

class CastCleanseMagicAction : public CastCureSpellAction
{
public:
    CastCleanseMagicAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "cleanse") {}
};

class CastCleansePoisonOnPartyAction : public CurePartyMemberAction
{
public:
    CastCleansePoisonOnPartyAction(PlayerbotAI* botAI) : CurePartyMemberAction(botAI, "cleanse", DISPEL_POISON) {}

    std::string const getName() override { return "cleanse poison on party"; }
};

class CastCleanseDiseaseOnPartyAction : public CurePartyMemberAction
{
public:
    CastCleanseDiseaseOnPartyAction(PlayerbotAI* botAI) : CurePartyMemberAction(botAI, "cleanse", DISPEL_DISEASE) {}

    std::string const getName() override { return "cleanse disease on party"; }
};

class CastCleanseMagicOnPartyAction : public CurePartyMemberAction
{
public:
    CastCleanseMagicOnPartyAction(PlayerbotAI* botAI) : CurePartyMemberAction(botAI, "cleanse", DISPEL_MAGIC) {}

    std::string const getName() override { return "cleanse magic on party"; }
};

BEGIN_SPELL_ACTION(CastAvengersShieldAction, "avenger's shield")
END_SPELL_ACTION()

BEGIN_SPELL_ACTION(CastExorcismAction, "exorcism")
END_SPELL_ACTION()

class CastHolyShieldAction : public CastBuffSpellAction
{
public:
    CastHolyShieldAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "holy shield") {}
};

class CastRedemptionAction : public ResurrectPartyMemberAction
{
public:
    CastRedemptionAction(PlayerbotAI* botAI) : ResurrectPartyMemberAction(botAI, "redemption") {}
};

class CastHammerOfJusticeOnEnemyHealerAction : public CastSpellOnEnemyHealerAction
{
public:
    CastHammerOfJusticeOnEnemyHealerAction(PlayerbotAI* botAI)
        : CastSpellOnEnemyHealerAction(botAI, "hammer of justice")
    {
    }
};

class CastHammerOfJusticeSnareAction : public CastSnareSpellAction
{
public:
    CastHammerOfJusticeSnareAction(PlayerbotAI* botAI) : CastSnareSpellAction(botAI, "hammer of justice") {}
};

class CastTurnUndeadAction : public CastBuffSpellAction
{
public:
    CastTurnUndeadAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "turn undead") {}

    Value<Unit*>* GetTargetValue() override;
};

PROTECT_ACTION(CastBlessingOfProtectionProtectAction, "blessing of protection");

class CastDivinePleaAction : public CastBuffSpellAction
{
public:
    CastDivinePleaAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "divine plea") {}
};

class CastInquisitionAction : public CastBuffSpellAction
{
public:
    CastInquisitionAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "inquisition") {}
    bool isUseful() override;
};

class ShieldOfRighteousnessAction : public CastMeleeSpellAction
{
public:
    ShieldOfRighteousnessAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "shield of righteousness") {}
};

class CastBeaconOfLightOnMainTankAction : public BuffOnMainTankAction
{
public:
    CastBeaconOfLightOnMainTankAction(PlayerbotAI* ai) : BuffOnMainTankAction(ai, "beacon of light", true) {}
};

class CastSacredShieldOnMainTankAction : public BuffOnMainTankAction
{
public:
    CastSacredShieldOnMainTankAction(PlayerbotAI* ai) : BuffOnMainTankAction(ai, "sacred shield", false) {}
};

class CastAvengingWrathAction : public CastBuffSpellAction
{
public:
    CastAvengingWrathAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "avenging wrath") {}
};

class CastHolyAvengerAction : public CastBuffSpellAction
{
public:
    CastHolyAvengerAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "holy avenger") {}
};

class CastGuardianOfAncientKingsAction : public CastBuffSpellAction
{
public:
    CastGuardianOfAncientKingsAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "guardian of ancient kings") {}
};

class CastDivineIlluminationAction : public CastBuffSpellAction
{
public:
    CastDivineIlluminationAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "divine illumination") {}
};

class CastDivineSacrificeAction : public CastBuffSpellAction
{
public:
    CastDivineSacrificeAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "divine sacrifice") {}
    bool isUseful() override;
};

class CastCancelDivineSacrificeAction : public Action
{
public:
    CastCancelDivineSacrificeAction(PlayerbotAI* botAI) : Action(botAI, "cancel divine sacrifice") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class CastTemplarsVerdictAction : public CastMeleeSpellAction
{
public:
    CastTemplarsVerdictAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "templar's verdict") {}
    bool isUseful() override;
};

class CastExecutionSentenceAction : public CastSpellAction
{
public:
    CastExecutionSentenceAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "execution sentence") {}
};

class CastLightsHammerAction : public CastSpellAction
{
public:
    CastLightsHammerAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "light's hammer") {}
};

class CastHolyPrismAction : public CastSpellAction
{
public:
    CastHolyPrismAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "holy prism") {}
};
#endif
