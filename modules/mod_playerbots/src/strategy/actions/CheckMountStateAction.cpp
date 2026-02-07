#include "CheckMountStateAction.h"

#include "BotFactory.h"
#include "DBCStores.h"
#include "Log.h"
#include "Playerbots.h"
#include "SpellMgr.h"
#include "TravelMgr.h"
#include "WorldPacket.h"

#include <algorithm>

bool CheckMountStateAction::isUseful()
{
    Player* master = botAI->GetMaster();
    bool masterMounted = master && master != bot &&
        (master->IsMounted() ||
         master->IsFlying() ||
         master->HasAuraType(SPELL_AURA_MOUNTED) ||
         master->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED) ||
         master->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK) ||
         master->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ||
         master->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS) ||
         master->HasAuraType(SPELL_AURA_FLY));

    auto infoLog = [&](char const* msg)
    {
        if (masterMounted)
            TC_LOG_INFO("server", "playerbots mount: %s (%s)", msg, bot->GetName().c_str());
    };

    if (botAI->IsInVehicle() || bot->isDead() || bot->HasUnitState(UNIT_STATE_IN_FLIGHT))
    {
        infoLog("not useful: vehicle/dead/in flight");
        return false;
    }

    if (!bot->IsOutdoors() && !masterMounted)
    {
        return false;
    }
    if (!bot->IsOutdoors() && masterMounted)
        infoLog("isUseful: indoors while master mounted, allowing sync attempt");

    if (bot->HasAuraType(SPELL_AURA_TRANSFORM) && bot->IsInDisallowedMountForm())
    {
        infoLog("not useful: disallowed mount form");
        return false;
    }

    if (!GET_PLAYERBOT_AI(bot)->HasStrategy("mount", BOT_STATE_NON_COMBAT) && !bot->IsMounted())
    {
        if (!masterMounted)
        {
            return false;
        }

        infoLog("isUseful: mount strategy disabled but master mounted, allowing sync");
    }

    if (bot->GetLevel() < sPlayerbotAIConfig->useGroundMountAtMinLevel)
    {
        infoLog("not useful: below mount level");
        return false;
    }

    return true;
}

bool CheckMountStateAction::Execute(Event /*event*/)
{
    auto infoLog = [&](char const* fmt, auto... args)
    {
        TC_LOG_INFO("server", fmt, args...);
    };

    // Keep travel mount thresholds tight: large dismount ranges cause mount/dismount loops.
    float dismountDistance = std::max(10.0f, sPlayerbotAIConfig->spellDistance + 2.0f);
    float mountDistance = std::max(21.0f, dismountDistance + 10.0f);

    Player* master = botAI->GetMaster();
    if (master && master != bot)
    {
        bool masterMounted = master->IsMounted() ||
            master->IsFlying() ||
            master->HasAuraType(SPELL_AURA_MOUNTED) ||
            master->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED) ||
            master->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK) ||
            master->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ||
            master->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS) ||
            master->HasAuraType(SPELL_AURA_FLY);

        if (masterMounted && !bot->IsMounted() && !bot->IsInCombat())
        {
            infoLog("playerbots mount: master mounted, trying mount (%s)", bot->GetName().c_str());
            return Mount();
        }

        if (!masterMounted && bot->IsMounted())
        {
            infoLog("playerbots mount: master dismounted, dismounting (%s)", bot->GetName().c_str());
            Dismount();
            return true;
        }

        // When following a master, do not use travel-distance dismount logic,
        // otherwise bots can mount and instantly dismount because travel distance
        // may be zero while staying near the master.
        return false;
    }

    float distance = 0.0f;
    TravelTarget* travelTarget = AI_VALUE(TravelTarget*, "travel target");
    if (travelTarget && travelTarget->isActive())
        distance = travelTarget->distance(bot);

    bool shouldMount = !bot->IsMounted() && distance > mountDistance && !bot->IsInCombat();
    bool shouldDismount = bot->IsMounted() && distance < dismountDistance;

    if (shouldDismount)
    {
        infoLog("playerbots mount: travel distance %.1f < %.1f, dismounting (%s)", distance, dismountDistance, bot->GetName().c_str());
        Dismount();
        return true;
    }

    if (shouldMount)
    {
        infoLog("playerbots mount: travel distance %.1f > %.1f, trying mount (%s)", distance, mountDistance, bot->GetName().c_str());
        return Mount();
    }

    return false;
}

bool CheckMountStateAction::Mount()
{
    auto infoLog = [&](char const* fmt, auto... args)
    {
        TC_LOG_INFO("server", fmt, args...);
    };

    botAI->RemoveShapeshift();

    if (bot->GetLevel() >= sPlayerbotAIConfig->useGroundMountAtMinLevel)
        bot->LearnSpell(33388, false);  // Apprentice Riding
    if (bot->GetLevel() >= sPlayerbotAIConfig->useFastGroundMountAtMinLevel)
        bot->LearnSpell(33391, false);  // Journeyman Riding
    if (bot->GetLevel() >= sPlayerbotAIConfig->useFlyMountAtMinLevel)
        bot->LearnSpell(34090, false);  // Expert Riding
    if (bot->GetLevel() >= sPlayerbotAIConfig->useFastFlyMountAtMinLevel)
        bot->LearnSpell(34091, false);  // Artisan Riding

    // Ensure older bots have required riding licenses when they attempt to mount.
    if (bot->GetLevel() >= sPlayerbotAIConfig->useFlyMountAtMinLevel)
    {
        bot->LearnSpell(54197, false);  // Cold Weather Flying
        bot->LearnSpell(90267, false);  // Flight Master's License
    }
    if (bot->GetLevel() >= 90)
        bot->LearnSpell(115913, false); // Wisdom of the Four Winds

    uint16 ridingValue = 0;
    if (bot->HasSpell(34091))
        ridingValue = 300;
    else if (bot->HasSpell(34090))
        ridingValue = 225;
    else if (bot->HasSpell(33391))
        ridingValue = 150;
    else if (bot->HasSpell(33388))
        ridingValue = 75;

    if (ridingValue && bot->GetSkillValue(SKILL_RIDING) < ridingValue)
    {
        uint16 skillStep = bot->GetSkillStep(SKILL_RIDING);
        bot->SetSkill(SKILL_RIDING, skillStep, ridingValue, ridingValue);
    }

    struct MountSpell
    {
        uint32 id = 0;
        int32 speed = 0;
        bool isFlight = false;
    };

    auto isGroundMountSpeedAura = [](uint32 auraType) -> bool
    {
        return auraType == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED ||
            auraType == SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS ||
            auraType == SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK;
    };

    auto isFlightMountSpeedAura = [](uint32 auraType) -> bool
    {
        return auraType == SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED ||
            auraType == SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS;
    };

    auto speedTier = [](int32 speed) -> int32
    {
        if (speed >= 279)
            return 279;
        if (speed >= 149)
            return 149;
        if (speed >= 99)
            return 99;
        if (speed >= 59)
            return 59;
        return 0;
    };

    auto fallbackRidingSpeed = [](Player const* player, bool isFlight) -> int32
    {
        if (isFlight)
        {
            if (player->HasSpell(34091))
                return 279;
            if (player->HasSpell(34090))
                return 149;
            return 0;
        }

        if (player->HasSpell(33391))
            return 99;
        if (player->HasSpell(33388))
            return 59;
        return 0;
    };

    auto ridingSpeedFromSkill = [](uint32 ridingSkill) -> int32
    {
        if (ridingSkill >= 300)
            return 279;
        if (ridingSkill >= 225)
            return 149;
        if (ridingSkill >= 150)
            return 99;
        if (ridingSkill >= 75)
            return 59;
        return 0;
    };

    auto capabilitySpeedFromSpell = [&](MountCapabilityEntry const* capability) -> int32
    {
        if (!capability)
            return 0;

        int32 capabilitySpeed = 0;
        if (SpellInfo const* speedSpell = sSpellMgr->GetSpellInfo(capability->SpeedModSpell))
        {
            for (uint8 i = EFFECT_0; i <= EFFECT_2; ++i)
            {
                uint32 auraType = speedSpell->Effects[i].ApplyAuraName;
                if (isGroundMountSpeedAura(auraType) ||
                    isFlightMountSpeedAura(auraType) ||
                    auraType == SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED ||
                    auraType == SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK)
                    capabilitySpeed = std::max(capabilitySpeed, speedSpell->Effects[i].BasePoints);
            }
        }

        if (capabilitySpeed <= 0)
            capabilitySpeed = ridingSpeedFromSkill(capability->RequiredRidingSkill);

        return capabilitySpeed;
    };

    auto capabilitySupportsFlight = [&](MountCapabilityEntry const* capability) -> bool
    {
        if (!capability || !capability->SpeedModSpell)
            return false;

        SpellInfo const* speedSpell = sSpellMgr->GetSpellInfo(capability->SpeedModSpell);
        if (!speedSpell)
            return false;

        for (uint8 i = EFFECT_0; i <= EFFECT_2; ++i)
        {
            uint32 auraType = speedSpell->Effects[i].ApplyAuraName;
            if (isFlightMountSpeedAura(auraType) ||
                auraType == SPELL_AURA_FLY ||
                auraType == SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED ||
                auraType == SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK)
                return true;
        }

        return false;
    };

    auto extractMountData = [&](Player const* player, SpellInfo const* spellInfo, bool& isMount, bool& isFlight, int32& speed)
    {
        isMount = false;
        isFlight = false;
        speed = 0;
        uint32 mountType = 0;

        for (uint8 i = EFFECT_0; i <= EFFECT_2; ++i)
        {
            uint32 auraType = spellInfo->Effects[i].ApplyAuraName;
            if (auraType == SPELL_AURA_MOUNTED)
            {
                isMount = true;
                if (spellInfo->Effects[i].MiscValueB > 0)
                    mountType = uint32(spellInfo->Effects[i].MiscValueB);
            }

            if (isGroundMountSpeedAura(auraType) || isFlightMountSpeedAura(auraType))
                speed = std::max(speed, spellInfo->Effects[i].BasePoints);

            if (isFlightMountSpeedAura(auraType) ||
                auraType == SPELL_AURA_FLY ||
                auraType == SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED ||
                auraType == SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK)
                isFlight = true;
        }

        // Many MoP mount spells derive flight/speed from selected mount capability, not direct spell auras.
        if (isMount && mountType)
        {
            bool hasAnyFlightCapability = false;
            int32 bestCapabilitySpeed = 0;
            if (MountTypeEntry const* mountTypeEntry = sMountTypeStore.LookupEntry(mountType))
            {
                uint32 ridingSkill = player->GetSkillValue(SKILL_RIDING);
                for (uint32 i = 0; i < MAX_MOUNT_CAPABILITIES; ++i)
                {
                    MountCapabilityEntry const* capability = sMountCapabilityStore.LookupEntry(mountTypeEntry->MountCapability[i]);
                    if (!capability)
                        continue;

                    if (ridingSkill < capability->RequiredRidingSkill)
                        continue;

                    if (capability->RequiredSpell && !player->HasSpell(capability->RequiredSpell))
                        continue;

                    bestCapabilitySpeed = std::max(bestCapabilitySpeed, capabilitySpeedFromSpell(capability));
                    if (capabilitySupportsFlight(capability))
                        hasAnyFlightCapability = true;
                }
            }

            if (MountCapabilityEntry const* capability = player->GetMountCapability(mountType))
            {
                speed = std::max(speed, capabilitySpeedFromSpell(capability));

                if (capabilitySupportsFlight(capability))
                    isFlight = true;
            }

            if (!isFlight && hasAnyFlightCapability)
                isFlight = true;

            if (speed <= 0)
                speed = bestCapabilitySpeed;
        }

        if (isMount && speed <= 0)
            speed = fallbackRidingSpeed(player, isFlight);
    };

    auto collectMountSpells = [&](Player* player, std::vector<MountSpell>& out)
    {
        for (auto const& entry : player->GetSpellMap())
        {
            uint32 spellId = entry.first;
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (!spellInfo)
                continue;

            if (entry.second->state == PLAYERSPELL_REMOVED || spellInfo->IsPassive())
                continue;

            bool isMount = false;
            bool isFlight = false;
            int32 speed = 0;
            extractMountData(player, spellInfo, isMount, isFlight, speed);

            if (!isMount)
                continue;

            out.push_back(MountSpell{ spellId, speed, isFlight });
        }
    };

    auto isMasterFlying = [&](Player* master) -> bool
    {
        if (!master)
            return false;

        if (master->IsFlying() || master->HasUnitState(UNIT_STATE_IN_FLIGHT))
            return true;

        if (master->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ||
            master->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED) ||
            master->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK) ||
            master->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS) ||
            master->HasAuraType(SPELL_AURA_FLY))
            return true;

        auto auraEffects = master->GetAuraEffectsByType(SPELL_AURA_MOUNTED);
        if (!auraEffects.empty())
        {
            for (AuraEffect const* mountAura : auraEffects)
            {
                SpellInfo const* masterSpell = mountAura->GetSpellInfo();
                if (!masterSpell)
                    continue;

                bool isMount = false;
                bool isFlight = false;
                int32 speed = 0;
                extractMountData(master, masterSpell, isMount, isFlight, speed);
                if (isMount && isFlight)
                    return true;
            }
        }

        return false;
    };

    auto getMasterSpeedTier = [&](Player* master) -> int32
    {
        if (!master || !(master->IsMounted() || master->HasAuraType(SPELL_AURA_MOUNTED)))
            return 0;

        auto auraEffects = master->GetAuraEffectsByType(SPELL_AURA_MOUNTED);
        if (auraEffects.empty())
            return 0;

        SpellInfo const* masterSpell = auraEffects.front()->GetSpellInfo();
        if (!masterSpell)
            return 0;

        bool isMount = false;
        bool isFlight = false;
        int32 speed = 0;
        extractMountData(master, masterSpell, isMount, isFlight, speed);
        return speedTier(speed);
    };

    Player* master = botAI->GetMaster();
    bool canFlyHere = bot->IsKnowHowFlyIn(bot->GetMapId(), bot->GetZoneId());
    bool masterMounted = master &&
        (master->IsMounted() ||
         master->IsFlying() ||
         master->HasAuraType(SPELL_AURA_MOUNTED) ||
         master->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED) ||
         master->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK) ||
         master->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ||
         master->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS) ||
         master->HasAuraType(SPELL_AURA_FLY));
    bool masterFlying = masterMounted ? isMasterFlying(master) : false;
    bool wantFlight = masterMounted ? (masterFlying && canFlyHere) : canFlyHere;
    int32 masterSpeedTier = getMasterSpeedTier(master);

    infoLog("playerbots mount: context canFly=%u masterMounted=%u masterFlying=%u wantFlight=%u masterSpeedTier=%d (%s)",
        canFlyHere ? 1u : 0u,
        masterMounted ? 1u : 0u,
        masterFlying ? 1u : 0u,
        wantFlight ? 1u : 0u,
        masterSpeedTier,
        bot->GetName().c_str());

    std::vector<MountSpell> mounts;
    collectMountSpells(bot, mounts);

    if (mounts.empty())
    {
        BotFactory factory(bot, bot->GetLevel());
        factory.InitSkills();
        factory.InitMounts();
        collectMountSpells(bot, mounts);
    }

    if (mounts.empty())
    {
        infoLog("playerbots mount: no mount spells after init (%s)", bot->GetName().c_str());
        return false;
    }

    std::vector<MountSpell> preferred;
    for (MountSpell const& mount : mounts)
    {
        if (mount.isFlight == wantFlight)
            preferred.push_back(mount);
    }

    // If we want to fly but have no detected flight mounts, try to initialize mounts once more.
    if (wantFlight && preferred.empty())
    {
        BotFactory factory(bot, bot->GetLevel());
        factory.InitSkills();
        factory.InitMounts();

        mounts.clear();
        collectMountSpells(bot, mounts);
        for (MountSpell const& mount : mounts)
        {
            if (mount.isFlight)
                preferred.push_back(mount);
        }
    }

    if (preferred.empty())
        preferred = mounts;

    auto meetsMasterSpeed = [&](MountSpell const& mount) -> bool
    {
        if (masterSpeedTier >= 279)
            return mount.speed >= 279;
        if (masterSpeedTier >= 149)
            return mount.isFlight ? mount.speed >= 149 : mount.speed >= 99;
        if (masterSpeedTier >= 99)
            return mount.speed >= 99;
        if (masterSpeedTier >= 59)
            return mount.speed >= 59;
        return true;
    };

    auto sortByBestSpeed = [](std::vector<MountSpell>& mountList)
    {
        std::sort(mountList.begin(), mountList.end(), [](MountSpell const& a, MountSpell const& b)
        {
            if (a.speed != b.speed)
                return a.speed > b.speed;
            // When speeds tie, prefer flight for flight requests and ground otherwise (handled by preferred list).
            return a.id < b.id;
        });
    };

    if (bot->isMoving())
        bot->StopMoving();

    if (!bot->IsStandState())
    {
        bot->SetStandState(UNIT_STAND_STATE_STAND);
        return false;
    }

    auto tryCastFrom = [&](std::vector<MountSpell> candidates, bool strictMasterSpeed) -> bool
    {
        sortByBestSpeed(candidates);
        for (MountSpell const& mount : candidates)
        {
            if (strictMasterSpeed && !meetsMasterSpeed(mount))
                continue;

            if (!sSpellMgr->GetSpellInfo(mount.id))
                continue;

            bool canCastCheck = botAI->CanCastSpell(mount.id, bot, false);
            bool hasSpell = bot->HasSpell(mount.id);
            infoLog("playerbots mount: casting mount spell %u speed %d flight %d (%s)",
                mount.id, mount.speed, mount.isFlight, bot->GetName().c_str());
            infoLog("playerbots mount: precheck spell %u hasSpell=%u canCast=%u (%s)",
                mount.id, hasSpell ? 1 : 0, canCastCheck ? 1 : 0, bot->GetName().c_str());
            bool casted = botAI->CastSpell(mount.id, bot);
            if (casted)
                return true;

            infoLog("playerbots mount: cast failed for %u (%s)", mount.id, bot->GetName().c_str());
        }
        return false;
    };

    if (tryCastFrom(preferred, true))
        return true;
    if (tryCastFrom(preferred, false))
        return true;
    if (preferred.size() != mounts.size())
    {
        if (tryCastFrom(mounts, true))
            return true;
        if (tryCastFrom(mounts, false))
            return true;
    }

    infoLog("playerbots mount: no castable mount spell found (%s)", bot->GetName().c_str());
    return false;
}

void CheckMountStateAction::Dismount()
{
    if (bot->isMoving())
        bot->StopMoving();

    WorldPacket emptyPacket;
    bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
}
