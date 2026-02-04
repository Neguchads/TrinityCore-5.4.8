#include "CheckMountStateAction.h"

#include "ItemVisitors.h"
#include "Playerbots.h"
#include "SpellMgr.h"
#include "TravelMgr.h"
#include "WorldPacket.h"

#include <algorithm>

bool CheckMountStateAction::isUseful()
{
    if (botAI->IsInVehicle() || bot->isDead() || bot->HasUnitState(UNIT_STATE_IN_FLIGHT))
        return false;

    if (!bot->IsOutdoors())
        return false;

    if (bot->HasAuraType(SPELL_AURA_TRANSFORM) && bot->IsInDisallowedMountForm())
        return false;

    if (!GET_PLAYERBOT_AI(bot)->HasStrategy("mount", BOT_STATE_NON_COMBAT) && !bot->IsMounted())
        return false;

    if (bot->GetLevel() < sPlayerbotAIConfig->useGroundMountAtMinLevel)
        return false;

    return true;
}

bool CheckMountStateAction::Execute(Event /*event*/)
{
    float mountDistance = std::max(40.0f, sPlayerbotAIConfig->farDistance);
    float dismountDistance = std::max(10.0f, sPlayerbotAIConfig->reactDistance);

    Player* master = botAI->GetMaster();
    if (master && master != bot)
    {
        float masterDistance = bot->GetExactDist(master);
        if (master->IsMounted() && !bot->IsMounted() && masterDistance > dismountDistance)
            return Mount();

        if (!master->IsMounted() && bot->IsMounted() && masterDistance < dismountDistance)
        {
            Dismount();
            return true;
        }
    }

    float distance = 0.0f;
    TravelTarget* travelTarget = AI_VALUE(TravelTarget*, "travel target");
    if (travelTarget && travelTarget->isActive())
        distance = travelTarget->distance(bot);

    bool shouldMount = !bot->IsMounted() && distance > mountDistance && !bot->IsInCombat();
    bool shouldDismount = bot->IsMounted() && distance < dismountDistance;

    if (shouldDismount)
    {
        Dismount();
        return true;
    }

    if (shouldMount)
        return Mount();

    return false;
}

bool CheckMountStateAction::Mount()
{
    botAI->RemoveShapeshift();

    struct MountSpell
    {
        uint32 id = 0;
        int32 speed = 0;
        bool isFlight = false;
    };

    auto collectMountSpells = [](Player* player, std::vector<MountSpell>& out)
    {
        for (auto const& entry : player->GetSpellMap())
        {
            uint32 spellId = entry.first;
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (!spellInfo)
                continue;

            if (entry.second->state == PLAYERSPELL_REMOVED || !entry.second->active || spellInfo->IsPassive())
                continue;

            if (spellInfo->Effects[0].ApplyAuraName != SPELL_AURA_MOUNTED)
                continue;

            int32 effect1 = spellInfo->Effects[1].BasePoints;
            int32 effect2 = spellInfo->Effects[2].BasePoints;
            int32 speed = std::max(effect1, effect2);

            bool isFlight = (spellInfo->Effects[1].ApplyAuraName == SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED ||
                             spellInfo->Effects[2].ApplyAuraName == SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED ||
                             spellInfo->Effects[1].ApplyAuraName == SPELL_AURA_FLY ||
                             spellInfo->Effects[2].ApplyAuraName == SPELL_AURA_FLY);

            out.push_back(MountSpell{ spellId, speed, isFlight });
        }
    };

    auto getMasterMountSpeed = [](Player* master) -> int32
    {
        if (!master)
            return 0;

        auto auraEffects = master->GetAuraEffectsByType(SPELL_AURA_MOUNTED);
        if (auraEffects.empty())
            return 0;

        SpellInfo const* masterSpell = auraEffects.front()->GetSpellInfo();
        if (!masterSpell)
            return 0;

        int32 effect1 = masterSpell->Effects[1].BasePoints;
        int32 effect2 = masterSpell->Effects[2].BasePoints;
        return std::max(effect1, effect2);
    };

    auto isMasterFlying = [](Player* master) -> bool
    {
        if (!master)
            return false;

        if (master->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) || master->HasAuraType(SPELL_AURA_FLY))
            return true;

        return false;
    };

    Player* master = botAI->GetMaster();
    bool wantFlight = master && master->IsMounted() && isMasterFlying(master);
    if (wantFlight && !bot->CanFly())
        wantFlight = false;

    int32 masterSpeed = getMasterMountSpeed(master);

    std::vector<MountSpell> mounts;
    collectMountSpells(bot, mounts);

    uint32 chosenMount = 0;
    int32 chosenSpeed = -1;
    for (MountSpell const& mount : mounts)
    {
        if (mount.isFlight != wantFlight)
            continue;

        if (masterSpeed > 0 && mount.speed > masterSpeed)
            continue;

        if (mount.speed > chosenSpeed)
        {
            chosenSpeed = mount.speed;
            chosenMount = mount.id;
        }
    }

    if (chosenMount && botAI->CanCastSpell(chosenMount, bot))
        return botAI->CastSpell(chosenMount, bot);

    FindMountVisitor visitor(bot);
    IterateItems(bot, visitor, ITERATE_ITEMS_IN_BAGS);
    std::vector<Item*>& items = visitor.GetResult();
    if (items.empty())
        return false;

    return UseItemOnSelf(items.front(), false);
}

void CheckMountStateAction::Dismount()
{
    if (bot->isMoving())
        bot->StopMoving();

    WorldPacket emptyPacket;
    bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
}
