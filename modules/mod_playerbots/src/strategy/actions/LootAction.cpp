/*
* This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "LootAction.h"

#include "Event.h"
#include "ItemPrototype.h"
#include "LootObjectStack.h"
#include "LootStrategyValue.h"
#include "ObjectMgr.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "QuestDef.h"
#include "ServerFacade.h"

bool LootAction::Execute(Event /*event*/)
{
    if (!AI_VALUE(bool, "has available loot"))
        return false;

    LootObject prevLoot = AI_VALUE(LootObject, "loot target");
    LootObject const& lootObject = AI_VALUE(LootObjectStack*, "available loot")->GetLoot(sPlayerbotAIConfig->lootDistance);

    if (lootObject.IsEmpty())
        return false;

    if (!prevLoot.IsEmpty() && prevLoot.guid != lootObject.guid)
    {
        WorldPacket* packet = new WorldPacket(CMSG_LOOT_RELEASE, 8);
        *packet << prevLoot.guid;
        bot->GetSession()->QueuePacket(packet);
    }

    botAI->GetAiObjectContext()->GetValue<LootObject>("loot target")->Set(lootObject);
    return true;
}

bool LootAction::isUseful()
{
    return !bot->GetGroup() || bot->GetGroup()->GetLootMethod() != FREE_FOR_ALL;
}

enum ProfessionSpells
{
    ALCHEMY = 2259,
    BLACKSMITHING = 2018,
    COOKING = 2550,
    ENCHANTING = 7411,
    ENGINEERING = 49383,
    FIRST_AID = 3273,
    FISHING = 7620,
    HERB_GATHERING = 2366,
    INSCRIPTION = 45357,
    JEWELCRAFTING = 25229,
    MINING = 2575,
    SKINNING = 8613,
    TAILORING = 3908
};

bool OpenLootAction::Execute(Event /*event*/)
{
    LootObject lootObject = AI_VALUE(LootObject, "loot target");
    bool result = DoLoot(lootObject);
    if (result)
    {
        AI_VALUE(LootObjectStack*, "available loot")->Remove(lootObject.guid);
        botAI->GetAiObjectContext()->GetValue<LootObject>("loot target")->Set(LootObject());
    }
    return result;
}

bool OpenLootAction::DoLoot(LootObject& lootObject)
{
    if (lootObject.IsEmpty())
        return false;

    Creature* creature = botAI->GetCreature(lootObject.guid);
    if (creature && bot->GetDistance(creature) > INTERACTION_DISTANCE - 2.0f)
        return false;

    if (bot->IsMounted())
    {
        bot->Dismount();
        botAI->SetNextCheckDelay(sPlayerbotAIConfig->lootDelay);
    }

    if (creature && creature->HasFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE))
    {
        WorldPacket* packet = new WorldPacket(CMSG_LOOT, 8);
        *packet << lootObject.guid;
        bot->GetSession()->QueuePacket(packet);
        botAI->SetNextCheckDelay(sPlayerbotAIConfig->lootDelay);
        return true;
    }

    if (bot->isMoving())
        bot->StopMoving();

    if (creature)
    {
        SkillType skill = creature->GetCreatureTemplate()->GetRequiredLootSkill();
        if (!CanOpenLock(skill, lootObject.reqSkillValue))
            return false;

        switch (skill)
        {
            case SKILL_ENGINEERING:
                return botAI->HasSkill(SKILL_ENGINEERING) ? botAI->CastSpell(ENGINEERING, creature) : false;
            case SKILL_HERBALISM:
                return botAI->HasSkill(SKILL_HERBALISM) ? botAI->CastSpell(32605, creature) : false;
            case SKILL_MINING:
                return botAI->HasSkill(SKILL_MINING) ? botAI->CastSpell(32606, creature) : false;
            default:
                return botAI->HasSkill(SKILL_SKINNING) ? botAI->CastSpell(SKINNING, creature) : false;
        }
    }

    GameObject* go = botAI->GetGameObject(lootObject.guid);
    if (go && bot->GetDistance(go) > INTERACTION_DISTANCE - 2.0f)
        return false;

    if (go && (go->GetGoState() != GO_STATE_READY))
        return false;

    if (go && go->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND))
        return false;

    if (go && go->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE))
        return false;

    if (lootObject.skillId == SKILL_MINING)
        return botAI->HasSkill(SKILL_MINING) ? botAI->CastSpell(MINING, bot) : false;

    if (lootObject.skillId == SKILL_HERBALISM)
        return botAI->HasSkill(SKILL_HERBALISM) ? botAI->CastSpell(HERB_GATHERING, bot) : false;

    uint32 spellId = GetOpeningSpell(lootObject);
    if (!spellId)
        return false;

    return botAI->CastSpell(spellId, bot);
}

uint32 OpenLootAction::GetOpeningSpell(LootObject& lootObject)
{
    if (GameObject* go = botAI->GetGameObject(lootObject.guid))
        if (go->isSpawned())
            return GetOpeningSpell(lootObject, go);

    return 0;
}

uint32 OpenLootAction::GetOpeningSpell(LootObject& lootObject, GameObject* go)
{
    for (PlayerSpellMap::iterator itr = bot->GetSpellMap().begin(); itr != bot->GetSpellMap().end(); ++itr)
    {
        uint32 spellId = itr->first;

        if (itr->second->state == PLAYERSPELL_REMOVED || !itr->second->active)
            continue;

        if (spellId == MINING || spellId == HERB_GATHERING)
            continue;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo)
            continue;

        if (CanOpenLock(lootObject, spellInfo, go))
            return spellId;
    }

    for (uint32 spellId = 0; spellId < sSpellMgr->GetSpellInfoStoreSize(); spellId++)
    {
        if (spellId == MINING || spellId == HERB_GATHERING)
            continue;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo)
            continue;

        if (CanOpenLock(lootObject, spellInfo, go))
            return spellId;
    }

    return sPlayerbotAIConfig->openGoSpell;
}

bool OpenLootAction::CanOpenLock(LootObject& /*lootObject*/, SpellInfo const* spellInfo, GameObject* go)
{
    for (uint8 effIndex = 0; effIndex <= EFFECT_2; effIndex++)
    {
        if (spellInfo->Effects[effIndex].Effect != SPELL_EFFECT_OPEN_LOCK &&
            spellInfo->Effects[effIndex].Effect != SPELL_EFFECT_SKINNING)
            return false;

        uint32 lockId = go->GetGOInfo()->GetLockId();
        if (!lockId)
            return false;

        LockEntry const* lockInfo = sLockStore.LookupEntry(lockId);
        if (!lockInfo)
            return false;

        for (uint8 j = 0; j < 8; ++j)
        {
            switch (lockInfo->Type[j])
            {
                case LOCK_KEY_SKILL:
                {
                    if (uint32(spellInfo->Effects[effIndex].MiscValue) != lockInfo->Index[j])
                        continue;

                    uint32 skillId = SkillByLockType(LockType(lockInfo->Index[j]));
                    if (skillId == SKILL_NONE)
                        return true;

                    if (CanOpenLock(skillId, lockInfo->Skill[j]))
                        return true;
                }
            }
        }
    }

    return false;
}

bool OpenLootAction::CanOpenLock(uint32 skillId, uint32 reqSkillValue)
{
    uint32 skillValue = bot->GetSkillValue(skillId);
    return skillValue >= reqSkillValue || !reqSkillValue;
}

bool StoreLootAction::Execute(Event /*event*/)
{
    ObjectGuid lootGuid = bot->GetLootGUID();
    if (!lootGuid)
        return false;

    Object* source = nullptr;
    Loot* loot = nullptr;

    if (lootGuid.IsCreature())
    {
        Creature* creature = botAI->GetCreature(lootGuid);
        if (creature)
        {
            source = creature;
            loot = &creature->loot;
        }
    }
    else if (lootGuid.IsGameObject())
    {
        GameObject* go = botAI->GetGameObject(lootGuid);
        if (go)
        {
            source = go;
            loot = &go->loot;
        }
    }
    else if (lootGuid.IsItem())
    {
        if (Item* item = bot->GetItemByGuid(lootGuid))
        {
            source = item;
            loot = &item->loot;
        }
    }

    if (!loot)
        return false;

    bool looted = false;

    if (loot->gold > 0)
    {
        WorldPacket* packet = new WorldPacket(CMSG_LOOT_MONEY, 0);
        bot->GetSession()->QueuePacket(packet);
        botAI->SetNextCheckDelay(sPlayerbotAIConfig->lootDelay);
        looted = true;
    }

    for (uint8 i = 0; i < loot->items.size(); ++i)
    {
        LootItem& item = loot->items[i];
        if (item.is_looted || item.is_blocked)
            continue;

        if (!item.AllowedForPlayer(bot, loot->GetSource()))
            continue;

        if (loot->loot_type != LOOT_SKINNING && !IsLootAllowed(item.itemid, botAI))
            continue;

        bot->StoreLootItem(i, loot, source);
        botAI->SetNextCheckDelay(sPlayerbotAIConfig->lootDelay);
        looted = true;
    }

    if (loot->isLooted())
    {
        bot->GetSession()->DoLootRelease(lootGuid);
        botAI->AccelerateRespawn(lootGuid);
    }

    if (looted)
    {
        AI_VALUE(LootObjectStack*, "available loot")->Remove(lootGuid);
        botAI->GetAiObjectContext()->GetValue<LootObject>("loot target")->Set(LootObject());
    }

    return looted;
}

bool StoreLootAction::IsLootAllowed(uint32 itemid, PlayerbotAI* botAI)
{
    AiObjectContext* ctx = botAI->GetAiObjectContext();
    LootStrategy* lootStrategy = ctx->GetValue<LootStrategy*>("loot strategy")->Get();

    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemid);
    if (!proto)
        return false;

    std::set<uint32>& lootItems = ctx->GetValue<std::set<uint32>&>("always loot list")->Get();
    if (lootItems.find(itemid) != lootItems.end())
        return true;

    uint32 max = proto->MaxCount;
    if (max > 0 && botAI->GetBot()->HasItemCount(itemid, max, true))
        return false;

    if (proto->StartQuest)
        return true;

    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 entry = botAI->GetBot()->GetQuestSlotQuestId(slot);
        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);
        if (!quest)
            continue;

        for (QuestObjective const& objective : quest->GetObjectives())
        {
            if (objective.Type == QUEST_OBJECTIVE_ITEM && objective.ObjectID == itemid)
                return true;
        }
    }

    return lootStrategy->CanLoot(proto, ctx);
}

bool ReleaseLootAction::Execute(Event /*event*/)
{
    GuidVector gos = botAI->GetAiObjectContext()->GetValue<GuidVector>("nearest game objects")->Get();
    for (ObjectGuid const guid : gos)
    {
        WorldPacket* packet = new WorldPacket(CMSG_LOOT_RELEASE, 8);
        *packet << guid;
        bot->GetSession()->QueuePacket(packet);
    }

    GuidVector corpses = botAI->GetAiObjectContext()->GetValue<GuidVector>("nearest corpses")->Get();
    for (ObjectGuid const guid : corpses)
    {
        WorldPacket* packet = new WorldPacket(CMSG_LOOT_RELEASE, 8);
        *packet << guid;
        bot->GetSession()->QueuePacket(packet);
        botAI->AccelerateRespawn(guid);
    }

    return true;
}
