

#ifndef _PLAYERBOT_SHAMANTRIGGERS_H
#define _PLAYERBOT_SHAMANTRIGGERS_H

#include "CureTriggers.h"
#include "GenericTriggers.h"
#include "SharedDefines.h"

class PlayerbotAI;

class FlameShockTrigger : public DebuffTrigger
{
public:
    FlameShockTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "flame shock", 1, true) {}
};

class LightningShieldTrigger : public BuffTrigger
{
public:
    LightningShieldTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "lightning shield") {}
};

class WaterShieldTrigger : public BuffTrigger
{
public:
    WaterShieldTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "water shield") {}
};

class LightningShieldChargesTrigger : public HasAuraStackTrigger
{
public:
    LightningShieldChargesTrigger(PlayerbotAI* botAI, uint8 minStacks)
        : HasAuraStackTrigger(botAI, "lightning shield", minStacks)
    {
    }
};

class MaelstromWeaponTrigger : public HasAuraStackTrigger
{
public:
    MaelstromWeaponTrigger(PlayerbotAI* botAI, uint8 minStacks)
        : HasAuraStackTrigger(botAI, "maelstrom weapon", minStacks)
    {
    }
};

class LavaSurgeTrigger : public HasAuraTrigger
{
public:
    LavaSurgeTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "lava surge") {}
};

class ElementalMasteryTrigger : public BuffTrigger
{
public:
    ElementalMasteryTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "elemental mastery") {}
};

class AscendanceTrigger : public BoostTrigger
{
public:
    AscendanceTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "ascendance") {}
};

class ElementalBlastTrigger : public SpellNoCooldownTrigger
{
public:
    ElementalBlastTrigger(PlayerbotAI* botAI) : SpellNoCooldownTrigger(botAI, "elemental blast") {}
};

class UnleashElementsTrigger : public SpellNoCooldownTrigger
{
public:
    UnleashElementsTrigger(PlayerbotAI* botAI) : SpellNoCooldownTrigger(botAI, "unleash elements") {}
};

class FireElementalTotemTrigger : public BoostTrigger
{
public:
    FireElementalTotemTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "fire elemental totem") {}
};

class EarthElementalTotemTrigger : public BoostTrigger
{
public:
    EarthElementalTotemTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "earth elemental totem") {}
};

class HealingTideTotemTrigger : public SpellNoCooldownTrigger
{
public:
    HealingTideTotemTrigger(PlayerbotAI* botAI) : SpellNoCooldownTrigger(botAI, "healing tide totem") {}
};

class SpiritLinkTotemTrigger : public SpellNoCooldownTrigger
{
public:
    SpiritLinkTotemTrigger(PlayerbotAI* botAI) : SpellNoCooldownTrigger(botAI, "spirit link totem") {}
};

class WindShearTrigger : public InterruptSpellTrigger
{
public:
    WindShearTrigger(PlayerbotAI* botAI) : InterruptSpellTrigger(botAI, "wind shear") {}
};

class WindShearEnemyHealerTrigger : public InterruptEnemyHealerTrigger
{
public:
    WindShearEnemyHealerTrigger(PlayerbotAI* botAI) : InterruptEnemyHealerTrigger(botAI, "wind shear") {}
};

class PurgeTrigger : public TargetAuraDispelTrigger
{
public:
    PurgeTrigger(PlayerbotAI* botAI) : TargetAuraDispelTrigger(botAI, "purge", DISPEL_MAGIC) {}
};

class CurePoisonTrigger : public NeedCureTrigger
{
public:
    CurePoisonTrigger(PlayerbotAI* botAI) : NeedCureTrigger(botAI, "cure poison", DISPEL_POISON) {}
};

class CurePoisonOnPartyTrigger : public PartyMemberNeedCureTrigger
{
public:
    CurePoisonOnPartyTrigger(PlayerbotAI* botAI) : PartyMemberNeedCureTrigger(botAI, "cure poison", DISPEL_POISON) {}
};

class CureDiseaseTrigger : public NeedCureTrigger
{
public:
    CureDiseaseTrigger(PlayerbotAI* botAI) : NeedCureTrigger(botAI, "cure disease", DISPEL_DISEASE) {}
};

class CureDiseaseOnPartyTrigger : public PartyMemberNeedCureTrigger
{
public:
    CureDiseaseOnPartyTrigger(PlayerbotAI* botAI) : PartyMemberNeedCureTrigger(botAI, "cure disease", DISPEL_DISEASE) {}
};

class CleanseSpiritPoisonTrigger : public NeedCureTrigger
{
public:
    CleanseSpiritPoisonTrigger(PlayerbotAI* botAI) : NeedCureTrigger(botAI, "cleanse spirit poison", DISPEL_POISON) {}
};

class CleanseSpiritPoisonOnPartyTrigger : public PartyMemberNeedCureTrigger
{
public:
    CleanseSpiritPoisonOnPartyTrigger(PlayerbotAI* botAI)
        : PartyMemberNeedCureTrigger(botAI, "cleanse spirit poison", DISPEL_POISON)
    {
    }
};

class CleanseSpiritDiseaseTrigger : public NeedCureTrigger
{
public:
    CleanseSpiritDiseaseTrigger(PlayerbotAI* botAI) : NeedCureTrigger(botAI, "cleanse spirit disease", DISPEL_DISEASE) {}
};

class CleanseSpiritDiseaseOnPartyTrigger : public PartyMemberNeedCureTrigger
{
public:
    CleanseSpiritDiseaseOnPartyTrigger(PlayerbotAI* botAI)
        : PartyMemberNeedCureTrigger(botAI, "cleanse spirit disease", DISPEL_DISEASE)
    {
    }
};

class CleanseSpiritCurseTrigger : public NeedCureTrigger
{
public:
    CleanseSpiritCurseTrigger(PlayerbotAI* botAI) : NeedCureTrigger(botAI, "cleanse spirit curse", DISPEL_CURSE) {}
};

class CleanseSpiritCurseOnPartyTrigger : public PartyMemberNeedCureTrigger
{
public:
    CleanseSpiritCurseOnPartyTrigger(PlayerbotAI* botAI)
        : PartyMemberNeedCureTrigger(botAI, "cleanse spirit curse", DISPEL_CURSE)
    {
    }
};

class HeroismTrigger : public BoostTrigger
{
public:
    HeroismTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "heroism") {}
};

class BloodlustTrigger : public BoostTrigger
{
public:
    BloodlustTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "bloodlust") {}
};

class TotemMissingTrigger : public Trigger
{
public:
    TotemMissingTrigger(PlayerbotAI* botAI, std::string const totem) : Trigger(botAI, totem), totem(totem) {}
    bool IsActive() override;

private:
    std::string totem;
};

class MainHandWeaponNoImbueTrigger : public Trigger
{
public:
    MainHandWeaponNoImbueTrigger(PlayerbotAI* botAI) : Trigger(botAI, "main hand weapon no imbue") {}
    bool IsActive() override;
};

class OffHandWeaponNoImbueTrigger : public Trigger
{
public:
    OffHandWeaponNoImbueTrigger(PlayerbotAI* botAI) : Trigger(botAI, "off hand weapon no imbue") {}
    bool IsActive() override;
};

class NoFireTotemTrigger : public Trigger
{
public:
    NoFireTotemTrigger(PlayerbotAI* botAI) : Trigger(botAI, "no fire totem") {}
    bool IsActive() override;
};

class NoAirTotemTrigger : public Trigger
{
public:
    NoAirTotemTrigger(PlayerbotAI* botAI) : Trigger(botAI, "no air totem") {}
    bool IsActive() override;
};

class NoWaterTotemTrigger : public Trigger
{
public:
    NoWaterTotemTrigger(PlayerbotAI* botAI) : Trigger(botAI, "no water totem") {}
    bool IsActive() override;
};

class NoEarthTotemTrigger : public Trigger
{
public:
    NoEarthTotemTrigger(PlayerbotAI* botAI) : Trigger(botAI, "no earth totem") {}
    bool IsActive() override;
};

#endif
