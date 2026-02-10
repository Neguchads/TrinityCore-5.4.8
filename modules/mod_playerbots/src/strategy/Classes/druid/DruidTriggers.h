
#ifndef _PLAYERBOT_DRUIDTRIGGERS_H
#define _PLAYERBOT_DRUIDTRIGGERS_H

#include "GenericTriggers.h"

class PlayerbotAI;

class MoonfireTrigger : public DebuffTrigger
{
public:
    MoonfireTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "moonfire", 1, true) {}
};

class SunfireTrigger : public DebuffTrigger
{
public:
    SunfireTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "sunfire", 1, true) {}
};

class ThrashTrigger : public SpellCanBeCastTrigger
{
public:
    ThrashTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "thrash") {}
};

class RakeTrigger : public DebuffTrigger
{
public:
    RakeTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "rake", 1, true) {}
};

class RipTrigger : public DebuffTrigger
{
public:
    RipTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "rip", 1, true) {}
};

class FaerieFireFeralTrigger : public DebuffTrigger
{
public:
    FaerieFireFeralTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "faerie fire (feral)", 1, true) {}
};

class SavageRoarTrigger : public BuffTrigger
{
public:
    SavageRoarTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "savage roar") {}
};

class TigersFuryTrigger : public SpellCanBeCastTrigger
{
public:
    TigersFuryTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "tiger's fury") {}
};

class BerserkTrigger : public BoostTrigger
{
public:
    BerserkTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "berserk") {}
};

class EnrageTrigger : public SpellCanBeCastTrigger
{
public:
    EnrageTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "enrage") {}
};

class MangleCatTrigger : public SpellCanBeCastTrigger
{
public:
    MangleCatTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "mangle (cat)") {}
};

class MangleBearTrigger : public SpellCanBeCastTrigger
{
public:
    MangleBearTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "mangle (bear)") {}
};

class LacerateTrigger : public DebuffTrigger
{
public:
    LacerateTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "lacerate", 1, true) {}
};

class InsectSwarmTrigger : public DebuffTrigger
{
public:
    InsectSwarmTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "insect swarm", 1, true) {}
};

class StarsurgeTrigger : public SpellCanBeCastTrigger
{
public:
    StarsurgeTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "starsurge") {}
};

class ShootingStarsTrigger : public HasAuraTrigger
{
public:
    ShootingStarsTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "shooting stars") {}
};

class StarfallTrigger : public SpellCanBeCastTrigger
{
public:
    StarfallTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "starfall") {}
};

class CelestialAlignmentTrigger : public SpellCanBeCastTrigger
{
public:
    CelestialAlignmentTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "celestial alignment") {}
};

class AstralCommunionTrigger : public SpellCanBeCastTrigger
{
public:
    AstralCommunionTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "astral communion") {}
};

class ForceOfNatureTrigger : public SpellCanBeCastTrigger
{
public:
    ForceOfNatureTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "force of nature") {}
};

class WildMushroomDetonateTrigger : public SpellCanBeCastTrigger
{
public:
    WildMushroomDetonateTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "wild mushroom: detonate") {}
};

class AstralStormTrigger : public SpellCanBeCastTrigger
{
public:
    AstralStormTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "astral storm") {}
};

class SolarBeamTrigger : public SpellCanBeCastTrigger
{
public:
    SolarBeamTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "solar beam") {}
};

class TyphoonTrigger : public SpellCanBeCastTrigger
{
public:
    TyphoonTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "typhoon") {}
};

class UrsolsVortexTrigger : public SpellCanBeCastTrigger
{
public:
    UrsolsVortexTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "ursol's vortex") {}
};

class MightOfUrsocTrigger : public SpellCanBeCastTrigger
{
public:
    MightOfUrsocTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "might of ursoc") {}
};

class SavageDefenseTrigger : public BuffTrigger
{
public:
    SavageDefenseTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "savage defense") {}
};

class FerociousBiteTimeTrigger : public Trigger
{
public:
    FerociousBiteTimeTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ferocious bite time", 1) {}
    bool IsActive() override;
};

class LifebloomOnMainTankTrigger : public BuffOnMainTankTrigger
{
public:
    LifebloomOnMainTankTrigger(PlayerbotAI* botAI) : BuffOnMainTankTrigger(botAI, "lifebloom") {}
};

class CenarionWardOnMainTankTrigger : public BuffOnMainTankTrigger
{
public:
    CenarionWardOnMainTankTrigger(PlayerbotAI* botAI) : BuffOnMainTankTrigger(botAI, "cenarion ward") {}
};

class GenesisTrigger : public SpellCanBeCastTrigger
{
public:
    GenesisTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "genesis") {}
};

class WildMushroomBloomTrigger : public SpellCanBeCastTrigger
{
public:
    WildMushroomBloomTrigger(PlayerbotAI* botAI) : SpellCanBeCastTrigger(botAI, "wild mushroom: bloom") {}
};

#endif
