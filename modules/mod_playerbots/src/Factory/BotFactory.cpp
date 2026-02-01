#include "BotFactory.h"

#include <algorithm>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>
 
#include "AccountMgr.h"
#include "AiFactory.h"
#include "ArenaTeam.h"
#include "Creature.h"
#include "DBCStores.h"
#include "DBCStructure.h"
#include "GuildMgr.h"
#include "Helper.h"
#include "Item.h"
#include "ItemVisitors.h"
#include "Log.h"
#include "LogCommon.h"
#include "LootMgr.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "PetDefines.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotSpec.h"
#include "Playerbots.h"
#include "RandomPlayerbotFactory.h"
#include "RandomItemManager.h"
#include "SharedDefines.h"
#include "SpellAuraDefines.h"
#include "SpellMgr.h"
#include "UseItemAction.h"
#include "World.h"

namespace
{
std::map<uint8, std::vector<uint32>> s_trainerIdCache;
std::vector<uint32> s_enchantSpellIdCache;
std::vector<uint32> s_enchantGemIdCache;
bool s_enchantCachesLoaded = false;
}
  
BotFactory::BotFactory(Player* bot, uint32 level, uint32 itemQuality, uint32 gearScoreLimit)
    : level(level), bot(bot)
{
    botAI = GET_PLAYERBOT_AI(bot);
}

void BotFactory::CancelAuras() { bot->RemoveAllAuras(); }
 
void BotFactory::Init()
{
    /*for (std::vector<uint32>::iterator i = sPlayerbotAIConfig->randomBotQuestIds.begin();
        i != sPlayerbotAIConfig->randomBotQuestIds.end(); ++i)
    {
        uint32 questId = *i;
        AddPrevQuests(questId, specialQuestIds);
        specialQuestIds.remove(questId);
        specialQuestIds.push_back(questId);
    }
    uint32 maxStoreSize = sSpellMgr->GetSpellInfoStoreSize();
    for (uint32 id = 1; id < maxStoreSize; ++id)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(id);
        if (!spellInfo)
            continue;
 
        if (id == 47181 || id == 50358 || id == 47242 || id == 52639 || id == 47147 || id == 7218)  // Test Enchant
            continue;
 
        uint32 RequiredLevel = spellInfo->BaseLevel;
 
        for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            if (spellInfo->Effects[j].Effect != SPELL_EFFECT_ENCHANT_ITEM)
                continue;
 
            uint32 enchant_id = spellInfo->Effects[j].MiscValue;
            if (!enchant_id)
                continue;
 
            SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
            if (!enchant || (enchant->Flags != PERM_ENCHANTMENT_SLOT && enchant->slot != TEMP_ENCHANTMENT_SLOT))
                continue;
 
            // SpellInfo const* enchantSpell = sSpellMgr->GetSpellInfo(enchant->spellid[0]);
            // if (!enchantSpell)
            //     continue;
            if (strstr(spellInfo->SpellName[0], "Test"))
                break;
 
            enchantSpellIdCache.push_back(id);
            break;
            // TC_LOG_INFO("playerbots", "Add {} to enchantment spells", id);
        }
    }
    TC_LOG_INFO("playerbots", "Loading {} enchantment spells", enchantSpellIdCache.size());
    for (auto iter = sSpellItemEnchantmentStore.begin(); iter != sSpellItemEnchantmentStore.end(); iter++)
    {
        uint32 gemId = iter->GemID;
        if (gemId == 0)
        {
            continue;
        }
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(gemId);
 
        if (proto->ItemLevel < 60)
            continue;
 
        if (proto->Flags & ITEM_FLAG_UNIQUE_EQUIPPABLE)
        {
            continue;
        }
        if (sRandomItemMgr->IsTestItem(gemId))
            continue;
 
        if (!proto || !sGemPropertiesStore.LookupEntry(proto->GemProperties))
        {
            continue;
        }
        // TC_LOG_INFO("playerbots", "Add {} to enchantment gems", gemId);
        enchantGemIdCache.push_back(gemId);
    }
    TC_LOG_INFO("playerbots", "Loading {} enchantment gems", enchantGemIdCache.size());*/
}
 
void BotFactory::Prepare()
{
    if (bot->isDead())
        bot->ResurrectPlayer(1.0f, false);
 
    bot->CombatStop(true);

    int32 newlevel = level;
    if (newlevel < 1)
        newlevel = 1;
    if (newlevel > 90)
        newlevel = 90;

    bot->GiveLevel(newlevel);
    bot->InitTalentForLevel();
    bot->SetUInt32Value(PLAYER_FIELD_XP, 0);
    bot->RemoveAllSpellCooldown();
    bot->InitStatsForLevel();
    CancelAuras();
}
 
void BotFactory::Randomize(bool incremental)
{
    TC_LOG_INFO("playerbots", "%s randomizing %s (level %u class = %s)...", (incremental ? "Incremental" : "Full"),
            bot->GetName().c_str(), level, ClassToString((Classes)bot->GetClass()).c_str());

    Prepare();
    if (!incremental)
    {
        // -- Unlearn talents and spec
        bot->ResetTalents(true, true, true);

        // -- release pet
        bot->RemovePet(PetRemoveMode::PET_REMOVE_ABANDON, PetRemoveFlag::PET_REMOVE_FLAG_NONE);

        // Destroy equipped items.
        for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
        {
            if (Item* item = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
            {
                std::string itemName = item->GetTemplate()->Name1;
                bot->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
            }
        }
    }

    InitPet();
 
    bot->SetMoney(urand(level * 100000, level * 5 * 100000));
    bot->SetHealth(bot->GetMaxHealth());
    bot->SetPower(POWER_MANA, bot->GetMaxPower(POWER_MANA));

    bot->SaveToDB(false);
}
 
void BotFactory::Refresh()
{
    InitPet();
    bot->DurabilityRepairAll(false, 1.0f, false);
    if (bot->isDead())
        bot->ResurrectPlayer(1.0f, false);
    uint32 money = urand(level * 1000, level * 5 * 1000);
    if (bot->GetMoney() < money)
        bot->SetMoney(money);
}
 
void BotFactory::InitPet()
{
    Pet* pet = bot->GetPet();

    /*if (bot->GetClass() == CLASS_HUNTER)
    {
        if (pet)
        {
            bot->RemovePet(PET_REMOVE_ABANDON);
            pet = nullptr;
        }

        // -- Delete all pet slots if possible
        for (uint8 pet_slot_active = 0; pet_slot_active < PetSlot::PET_SLOT_ACTIVE_LAST; ++pet_slot_active)
        {
            uint32 pet_id = bot->GetPetIdBySlot(pet_slot_active);
            if (!pet_id) continue;

            bot->SummonPet(pet_id, bot->GetWorldLocation().GetPositionX(), bot->GetWorldLocation().GetPositionY(),
                           bot->GetWorldLocation().GetPositionZ(), 0.0f, 0);
            bot->RemovePet(PetRemoveMode::PET_REMOVE_ABANDON);
            pet = nullptr;
        }
    }*/

    if (!pet)
    {
        if (bot->GetClass() != CLASS_HUNTER)
            return;
 
        Map* map = bot->GetMap();
        if (!map)
            return;

        std::vector<uint32> ids;
        CreatureTemplateContainer const* creatures = sObjectMgr->GetCreatureTemplates();
        for (CreatureTemplateContainer::const_iterator itr = creatures->begin(); itr != creatures->end(); ++itr)
        {
            if (!itr->second.IsTameable(bot->CanTameExoticPets()))
                continue;
 
            if (itr->second.minlevel > bot->GetLevel())
                continue;
 
            ids.push_back(itr->first);
        }
 
        if (ids.empty())
        {
            TC_LOG_ERROR("playerbots", "No pets available for bot %s (%u level)", bot->GetName().c_str(), bot->GetLevel());
            return;
        }

        for (uint32 i = 0; i < 10; i++)
        {
            uint32 index = urand(0, ids.size() - 1);
            CreatureTemplate const* co = sObjectMgr->GetCreatureTemplate(ids[index]);
            if (!co)
                continue;
            if (co->Name.size() > 21)
                continue;

            int8 newPetSlot = bot->GetSlotForNewPet();
            if (newPetSlot == -1)
                continue;

            // Everything looks OK, create new pet
            pet = bot->CreateTamedPetFrom(co->Entry, 0);
            if (!pet)
                continue;

            // prepare visual effect for levelup
            pet->SetUInt32Value(UNIT_FIELD_LEVEL, bot->GetLevel() - 1);
 
            // add to world
            pet->GetMap()->AddToMap(pet->ToCreature());
 
            // visual effect for levelup
            pet->SetUInt32Value(UNIT_FIELD_LEVEL, bot->GetLevel());
 
            // caster have pet now
            bot->SetMinion(pet, true);
 
            pet->InitTalentForLevel();
 
            pet->SavePetToDB();
            bot->PetSpellInitialize();
            break;
        }
    }
 
    if (pet)
    {
        pet->InitStatsForLevel(bot->GetLevel());
        pet->SetLevel(bot->GetLevel());
        pet->SetHealth(pet->GetMaxHealth());
    }
    else
    {
        TC_LOG_ERROR("playerbots", "Cannot create pet for bot %s", bot->GetName().c_str());
        return;
    }
 
    // TC_LOG_INFO("playerbots", "Start make spell auto cast for {} spells. {} already auto casted.", pet->m_spells.size(),
    // pet->GetPetAutoSpellSize());
    for (PetSpellMap::const_iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end(); ++itr)
    {
        if (itr->second.state == PETSPELL_REMOVED)
            continue;
 
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
        if (!spellInfo)
            continue;
 
        if (spellInfo->IsPassive())
        {
            continue;
        }
        pet->ToggleAutocast(spellInfo, true);
    }
}
#include <fstream>
void BotFactory::InitTalentsTree(bool reset)
{
    /*std::map<uint32, std::list<const TalentEntry*>> talents_dbc;
    for (auto entry = sTalentStore.begin(); entry != sTalentStore.end(); ++entry)
    {
        if (talents_dbc.find(entry->PlayerClass) == talents_dbc.end())
            talents_dbc[entry->PlayerClass] = std::list<const TalentEntry*>();
        talents_dbc[entry->PlayerClass].push_back(*entry);
    }

    for (auto& ref : talents_dbc)
    {
        ref.second.sort([](const TalentEntry* a, const TalentEntry* b)
        {
            return (a->Row < b->Row) || (a->Row == b->Row && a->Col < b->Col);
        });
    }

    std::ofstream os("./talent_export.txt", std::ios::app);
    for (const auto& ref : talents_dbc)
    {
        auto classe = ClassToString((Classes)ref.first);
        os << classe << ":\n";
        uint32 currentRow = 0;
        for (const auto& tal : ref.second)
        {
            if (tal->Row != currentRow)
            {
                currentRow = tal->Row;
                os << "\n";
            }
            os << tal->TalentID << "\t";
        }
        os << "\n";
    }
    os.close();*/

    // -- reset spec in case we down level
    if (reset)
    {
        bot->ResetTalents(true, true, true);
    }
    
    // if no spec then pick one random (need to change that to balance)
    if (bot->GetSpecialization() == Specializations::SPEC_NONE)
    {
        // -- Select spec
        if (bot->GetLevel() >= 10)
        {
            uint32 tab = std::rand() % 3;
            WorldPacket p(CMSG_SET_PRIMARY_TALENT_TREE);
            p << tab;
            bot->GetSession()->HandeSetTalentSpecialization(p);
            bot->ActivateSpec(0);
        }
    }

    WorldPacket p(CMSG_LEARN_TALENT);
    uint32 alreadyUsedPoints = bot->GetUsedTalentCount();
    uint8 spec_tab = PlayerBotSpec::GetSpectab(bot);
    uint32 availablepoints = bot->CalculateTalentsPoints() - bot->GetUsedTalentCount();
    uint32 learnCount = 0;

    if (!availablepoints || spec_tab == 99) return;

    const std::vector<uint16>& talents = sPlayerbotAIConfig->premadeSpecLink[bot->GetClass()][spec_tab];
    if (!talents.empty())
    {
        std::vector<uint16> talent_to_learn;
        for (size_t i = alreadyUsedPoints; i < talents.size() && availablepoints > 0; ++i)
        {
            uint16 talentId = talents[i];
            if (!bot->HasTalent(talentId, bot->GetActiveSpec()))
            {
                learnCount++;
                talent_to_learn.push_back(talentId);
                availablepoints--;
            }
        }
        if (learnCount > 0)
        {
            p.WriteBits(learnCount, 23);
            for (const auto& c : talent_to_learn)
                p << c;
            bot->GetSession()->HandleLearnTalentOpcode(p);
        }
        return;
    }

    // Fallback for MoP talent system: pick one talent per available tier
    uint32 maxRows = bot->GetUInt32Value(PLAYER_FIELD_MAX_TALENT_TIERS);
    if (!maxRows)
        maxRows = bot->CalculateTalentsPoints();
    if (!maxRows)
        return;

    std::unordered_map<uint32, std::vector<uint32>> talentsByRow;
    for (uint32 i = 0; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo)
            continue;
        if (talentInfo->PlayerClass != bot->GetClass())
            continue;
        if (talentInfo->Row > maxRows)
            continue;
        talentsByRow[talentInfo->Row].push_back(talentInfo->TalentID);
    }

    for (uint32 row = 0; row <= maxRows && availablepoints > 0; ++row)
    {
        auto it = talentsByRow.find(row);
        if (it == talentsByRow.end() || it->second.empty())
            continue;

        std::vector<uint32>& options = it->second;
        std::shuffle(options.begin(), options.end(), std::mt19937(std::random_device{}()));
        for (uint32 talentId : options)
        {
            if (bot->LearnTalent(talentId))
            {
                availablepoints--;
                break;
            }
        }
    }
}

void BotFactory::InitSkills()
{
    if (!bot)
        return;

    bot->UpdateSkillsForLevel();
    bot->SetSkill(SKILL_RIDING, 0, 0, 0);
    if (bot->GetLevel() >= sPlayerbotAIConfig->useGroundMountAtMinLevel)
        bot->LearnSpell(33388, false);
    if (bot->GetLevel() >= sPlayerbotAIConfig->useFastGroundMountAtMinLevel)
        bot->LearnSpell(33391, false);
    if (bot->GetLevel() >= sPlayerbotAIConfig->useFlyMountAtMinLevel)
        bot->LearnSpell(34090, false);
    if (bot->GetLevel() >= sPlayerbotAIConfig->useFastFlyMountAtMinLevel)
        bot->LearnSpell(34091, false);

    uint32 skillLevel = bot->GetLevel() < 40 ? 0 : 1;
    uint32 dualWieldLevel = bot->GetLevel() < 20 ? 0 : 1;
    SetRandomSkill(SKILL_DEFENSE);
    SetRandomSkill(SKILL_UNARMED);
    switch (bot->GetClass())
    {
        case CLASS_DRUID:
            SetRandomSkill(SKILL_MACES);
            SetRandomSkill(SKILL_STAVES);
            SetRandomSkill(SKILL_2H_MACES);
            SetRandomSkill(SKILL_DAGGERS);
            SetRandomSkill(SKILL_POLEARMS);
            SetRandomSkill(SKILL_FIST_WEAPONS);
            break;
        case CLASS_WARRIOR:
            SetRandomSkill(SKILL_SWORDS);
            SetRandomSkill(SKILL_AXES);
            SetRandomSkill(SKILL_BOWS);
            SetRandomSkill(SKILL_GUNS);
            SetRandomSkill(SKILL_MACES);
            SetRandomSkill(SKILL_2H_SWORDS);
            SetRandomSkill(SKILL_STAVES);
            SetRandomSkill(SKILL_2H_MACES);
            SetRandomSkill(SKILL_2H_AXES);
            SetRandomSkill(SKILL_DAGGERS);
            SetRandomSkill(SKILL_CROSSBOWS);
            SetRandomSkill(SKILL_POLEARMS);
            SetRandomSkill(SKILL_FIST_WEAPONS);
            SetRandomSkill(SKILL_THROWN);
            bot->SetSkill(SKILL_DUAL_WIELD, 0, dualWieldLevel, dualWieldLevel);
            bot->SetSkill(SKILL_PLATE_MAIL, 0, skillLevel, skillLevel);
            bot->SetCanDualWield(dualWieldLevel);
            break;
        case CLASS_PALADIN:
            SetRandomSkill(SKILL_SWORDS);
            SetRandomSkill(SKILL_AXES);
            SetRandomSkill(SKILL_MACES);
            SetRandomSkill(SKILL_2H_SWORDS);
            SetRandomSkill(SKILL_2H_MACES);
            SetRandomSkill(SKILL_2H_AXES);
            SetRandomSkill(SKILL_POLEARMS);
            bot->SetSkill(SKILL_PLATE_MAIL, 0, skillLevel, skillLevel);
            break;
        case CLASS_PRIEST:
            SetRandomSkill(SKILL_MACES);
            SetRandomSkill(SKILL_STAVES);
            SetRandomSkill(SKILL_DAGGERS);
            SetRandomSkill(SKILL_WANDS);
            break;
        case CLASS_SHAMAN:
            SetRandomSkill(SKILL_AXES);
            SetRandomSkill(SKILL_MACES);
            SetRandomSkill(SKILL_STAVES);
            SetRandomSkill(SKILL_2H_MACES);
            SetRandomSkill(SKILL_2H_AXES);
            SetRandomSkill(SKILL_DAGGERS);
            SetRandomSkill(SKILL_FIST_WEAPONS);
            bot->SetSkill(SKILL_MAIL, 0, skillLevel, skillLevel);
            break;
        case CLASS_MAGE:
            SetRandomSkill(SKILL_SWORDS);
            SetRandomSkill(SKILL_STAVES);
            SetRandomSkill(SKILL_DAGGERS);
            SetRandomSkill(SKILL_WANDS);
            break;
        case CLASS_WARLOCK:
            SetRandomSkill(SKILL_SWORDS);
            SetRandomSkill(SKILL_STAVES);
            SetRandomSkill(SKILL_DAGGERS);
            SetRandomSkill(SKILL_WANDS);
            break;
        case CLASS_HUNTER:
            SetRandomSkill(SKILL_SWORDS);
            SetRandomSkill(SKILL_AXES);
            SetRandomSkill(SKILL_BOWS);
            SetRandomSkill(SKILL_GUNS);
            SetRandomSkill(SKILL_2H_SWORDS);
            SetRandomSkill(SKILL_STAVES);
            SetRandomSkill(SKILL_2H_AXES);
            SetRandomSkill(SKILL_DAGGERS);
            SetRandomSkill(SKILL_CROSSBOWS);
            SetRandomSkill(SKILL_POLEARMS);
            SetRandomSkill(SKILL_FIST_WEAPONS);
            SetRandomSkill(SKILL_THROWN);
            bot->SetSkill(SKILL_DUAL_WIELD, 0, dualWieldLevel, dualWieldLevel);
            bot->SetSkill(SKILL_MAIL, 0, skillLevel, skillLevel);
            bot->SetCanDualWield(dualWieldLevel);
            break;
        case CLASS_ROGUE:
            SetRandomSkill(SKILL_SWORDS);
            SetRandomSkill(SKILL_AXES);
            SetRandomSkill(SKILL_BOWS);
            SetRandomSkill(SKILL_GUNS);
            SetRandomSkill(SKILL_MACES);
            SetRandomSkill(SKILL_DAGGERS);
            SetRandomSkill(SKILL_CROSSBOWS);
            SetRandomSkill(SKILL_FIST_WEAPONS);
            SetRandomSkill(SKILL_THROWN);
            SetRandomSkill(SKILL_LOCKPICKING);
            bot->SetSkill(SKILL_DUAL_WIELD, 0, 1, 1);
            bot->SetCanDualWield(true);
            break;
        case CLASS_DEATH_KNIGHT:
            SetRandomSkill(SKILL_SWORDS);
            SetRandomSkill(SKILL_AXES);
            SetRandomSkill(SKILL_MACES);
            SetRandomSkill(SKILL_2H_SWORDS);
            SetRandomSkill(SKILL_2H_MACES);
            SetRandomSkill(SKILL_2H_AXES);
            SetRandomSkill(SKILL_POLEARMS);
            bot->SetSkill(SKILL_DUAL_WIELD, 0, dualWieldLevel, dualWieldLevel);
            bot->SetCanDualWield(dualWieldLevel);
            break;
        default:
            break;
    }
}

void BotFactory::SetRandomSkill(uint16 id)
{
    uint32 maxValue = level * 5;
    uint32 value = maxValue;
    bot->SetSkill(id, 0, value, maxValue);
}

void BotFactory::InitAvailableSpells()
{
    if (s_trainerIdCache[bot->GetClass()].empty())
    {
        CreatureTemplateContainer const* creatureTemplateContainer = sObjectMgr->GetCreatureTemplates();
        for (CreatureTemplateContainer::const_iterator i = creatureTemplateContainer->begin();
             i != creatureTemplateContainer->end(); ++i)
        {
            if (!(i->second.npcflag & UNIT_NPC_FLAG_TRAINER))
                continue;
            TrainerSpellData const* trainerSpells = sObjectMgr->GetNpcTrainerSpells(i->first);
            if (!trainerSpells)
                continue;
            s_trainerIdCache[bot->GetClass()].push_back(i->first);
        }
    }

    for (uint32 trainerId : s_trainerIdCache[bot->GetClass()])
    {
        TrainerSpellData const* trainerSpells = sObjectMgr->GetNpcTrainerSpells(trainerId);
        if (!trainerSpells)
            continue;
        for (auto const& itr : trainerSpells->spellList)
        {
            TrainerSpell const* trainerSpell = &itr.second;
            if (!trainerSpell)
                continue;
            if (trainerSpell->reqLevel > bot->GetLevel())
                continue;
            TrainerSpellState state = bot->GetTrainerSpellState(trainerSpell);
            if (state != TRAINER_SPELL_GREEN)
                continue;
            if (trainerSpell->IsCastable())
                bot->CastSpell(bot, trainerSpell->spell, true);
            else
                bot->LearnSpell(trainerSpell->spell, false);
        }
    }
}

void BotFactory::InitClassSpells()
{
    int32_t curLevel = bot->GetLevel();
    switch (bot->GetClass())
    {
        case CLASS_WARRIOR:
            bot->LearnSpell(78, true);
            bot->LearnSpell(2457, true);
            if (curLevel >= 10)
            {
                bot->LearnSpell(71, false);
                bot->LearnSpell(355, false);
                bot->LearnSpell(7386, false);
            }
            if (curLevel >= 30)
            {
                bot->LearnSpell(2458, false);
            }
            break;
        case CLASS_PALADIN:
            bot->LearnSpell(21084, true);
            bot->LearnSpell(635, true);
            if (curLevel >= 12)
                bot->LearnSpell(7328, false);
            break;
        case CLASS_ROGUE:
            bot->LearnSpell(1752, true);
            bot->LearnSpell(2098, true);
            break;
        case CLASS_DEATH_KNIGHT:
            bot->LearnSpell(45477, true);
            bot->LearnSpell(47541, true);
            bot->LearnSpell(45462, true);
            bot->LearnSpell(45902, true);
            bot->LearnSpell(53428, false);
            bot->LearnSpell(50977, false);
            bot->LearnSpell(49142, false);
            bot->LearnSpell(48778, false);
            break;
        case CLASS_HUNTER:
            bot->LearnSpell(2973, true);
            bot->LearnSpell(75, true);
            if (curLevel >= 10)
            {
                bot->LearnSpell(883, false);
                bot->LearnSpell(1515, false);
                bot->LearnSpell(6991, false);
                bot->LearnSpell(982, false);
                bot->LearnSpell(2641, false);
            }
            break;
        case CLASS_PRIEST:
            bot->LearnSpell(585, true);
            bot->LearnSpell(2050, true);
            break;
        case CLASS_MAGE:
            bot->LearnSpell(133, true);
            bot->LearnSpell(168, true);
            break;
        case CLASS_WARLOCK:
            bot->LearnSpell(687, true);
            bot->LearnSpell(686, true);
            bot->LearnSpell(688, false);
            if (curLevel >= 10)
                bot->LearnSpell(697, false);
            if (curLevel >= 20)
                bot->LearnSpell(712, false);
            if (curLevel >= 30)
                bot->LearnSpell(691, false);
            break;
        case CLASS_DRUID:
            bot->LearnSpell(5176, true);
            bot->LearnSpell(5185, true);
            if (curLevel >= 10)
            {
                bot->LearnSpell(5487, false);
                bot->LearnSpell(6795, false);
                bot->LearnSpell(6807, false);
            }
            break;
        case CLASS_SHAMAN:
            bot->LearnSpell(403, true);
            bot->LearnSpell(331, true);
            if (curLevel >= 4)
                bot->LearnSpell(8071, false);
            if (curLevel >= 10)
                bot->LearnSpell(3599, false);
            if (curLevel >= 20)
                bot->LearnSpell(5394, false);
            break;
        default:
            break;
    }
}

void BotFactory::InitSpecialSpells()
{
    for (uint32 spellId : sPlayerbotAIConfig->randomBotSpellIds)
        bot->LearnSpell(spellId, false);
    if (bot->GetClass() == CLASS_DEATH_KNIGHT)
        bot->LearnSpell(50977, false);
}

void BotFactory::InitReagents()
{
    std::vector<std::pair<uint32, uint32>> items;
    switch (bot->GetClass())
    {
        case CLASS_DEATH_KNIGHT:
            if (level >= 56)
                items.push_back({37201, 40});
            break;
        case CLASS_DRUID:
            if (level >= 20 && level < 30)
                items.push_back({17034, 20});
            else if (level >= 30 && level < 40)
                items.push_back({17035, 20});
            else if (level >= 40 && level < 50)
                items.push_back({17036, 20});
            else if (level >= 50 && level < 60)
            {
                items.push_back({17037, 20});
                items.push_back({17021, 20});
            }
            else if (level >= 60 && level < 69)
            {
                items.push_back({17038, 20});
                items.push_back({17026, 20});
            }
            else if (level == 69)
            {
                items.push_back({22147, 20});
                items.push_back({17026, 20});
            }
            else if (level >= 70 && level < 79)
            {
                items.push_back({22147, 20});
                items.push_back({22148, 20});
            }
            else if (level == 79)
            {
                items.push_back({44614, 20});
                items.push_back({22148, 20});
            }
            else if (level >= 80)
            {
                items.push_back({44614, 20});
                items.push_back({44605, 20});
            }
            break;
        case CLASS_MAGE:
            if (level >= 20)
                items.push_back({17031, 20});
            if (level >= 40)
                items.push_back({17032, 20});
            if (level >= 56)
                items.push_back({17020, 20});
            break;
        case CLASS_PALADIN:
            if (level >= 52)
                items.push_back({21177, 80});
            break;
        case CLASS_PRIEST:
            if (level >= 48 && level < 56)
                items.push_back({17028, 40});
            else if (level >= 56 && level < 60)
            {
                items.push_back({17028, 20});
                items.push_back({17029, 20});
            }
            else if (level >= 60 && level < 77)
                items.push_back({17029, 40});
            else if (level >= 77 && level < 80)
            {
                items.push_back({17029, 20});
                items.push_back({44615, 20});
            }
            else if (level >= 80)
                items.push_back({44615, 40});
            break;
        case CLASS_SHAMAN:
            if (level >= 4)
                items.push_back({5175, 1});
            if (level >= 10)
                items.push_back({5176, 1});
            if (level >= 20)
                items.push_back({5177, 1});
            if (level >= 30)
            {
                items.push_back({5178, 1});
                items.push_back({17030, 20});
            }
            break;
        case CLASS_WARLOCK:
            items.push_back({6265, 5});
            break;
        default:
            break;
    }

    for (auto const& item : items)
        StoreItem(item.first, item.second);
}

void BotFactory::InitConsumables()
{
    // MoP: add flasks (no elixirs/oils/weightstones).
    if (!bot)
        return;

    std::vector<uint32> flaskIds = { 76084, 76085, 76086, 76087, 76088 };
    for (uint32 itemId : flaskIds)
    {
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
        if (!proto)
            continue;
        if (proto->RequiredLevel > bot->GetLevel())
            continue;
        StoreItem(itemId, std::max<uint32>(1, proto->GetMaxStackSize()));
        break;
    }

    std::vector<uint32> combatConsumables;
    std::vector<uint32> bandages;
    ItemTemplateContainer const* itemTemplateContainer = sObjectMgr->GetItemTemplateStore();
    for (auto const& itr : *itemTemplateContainer)
    {
        ItemTemplate const* proto = &itr.second;
        if (!proto)
            continue;
        if (proto->Class != ITEM_CLASS_CONSUMABLE)
            continue;
        if (proto->SubClass == ITEM_SUBCLASS_FOOD_DRINK)
            continue;
        if (proto->RequiredLevel > bot->GetLevel() || proto->RequiredLevel < bot->GetLevel() - 9)
            continue;
        if (proto->Bonding != BIND_NONE)
            continue;

        bool isBandage = (proto->SubClass == ITEM_SUBCLASS_BANDAGE);
        bool isCombat = (proto->SubClass == ITEM_SUBCLASS_POTION || proto->SubClass == ITEM_SUBCLASS_ELIXIR ||
                         proto->SubClass == ITEM_SUBCLASS_SCROLL || proto->SubClass == ITEM_SUBCLASS_CONSUMABLE ||
                         proto->SubClass == ITEM_SUBCLASS_CONSUMABLE_OTHER || proto->SubClass == ITEM_SUBCLASS_ITEM_ENHANCEMENT);

        bool matches = false;
        for (uint8 j = 0; j < MAX_ITEM_PROTO_SPELLS && !matches; ++j)
        {
            uint32 spellId = proto->Spells[j].SpellId;
            if (!spellId || proto->Spells[j].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
                continue;
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (!spellInfo)
                continue;

            for (uint8 i = 0; i < 3; ++i)
            {
                if (isBandage)
                {
                    if (spellInfo->Effects[i].Effect == SPELL_EFFECT_HEAL ||
                        (spellInfo->Effects[i].Effect == SPELL_EFFECT_APPLY_AURA &&
                         spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_PERIODIC_HEAL))
                    {
                        matches = true;
                        break;
                    }
                }
                else if (isCombat)
                {
                    if (spellInfo->Effects[i].Effect == SPELL_EFFECT_APPLY_AURA)
                    {
                        uint32 aura = spellInfo->Effects[i].ApplyAuraName;
                        if (aura == SPELL_AURA_MOD_STAT ||
                            aura == SPELL_AURA_MOD_ATTACK_POWER ||
                            aura == SPELL_AURA_MOD_RANGED_ATTACK_POWER ||
                            aura == SPELL_AURA_MOD_MELEE_HASTE ||
                            aura == SPELL_AURA_MOD_RATING ||
                            aura == SPELL_AURA_MOD_CRIT_PCT ||
                            aura == SPELL_AURA_MOD_INCREASE_HEALTH ||
                            aura == SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT ||
                            aura == SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT ||
                            aura == SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT ||
                            aura == SPELL_AURA_MOD_SPELL_POWER_PCT)
                        {
                            matches = true;
                            break;
                        }
                    }
                }
            }
        }

        if (!matches)
            continue;

        if (isBandage)
            bandages.push_back(proto->ItemId);
        else if (isCombat)
            combatConsumables.push_back(proto->ItemId);
    }

    if (!combatConsumables.empty())
    {
        for (uint8 i = 0; i < 3 && !combatConsumables.empty(); ++i)
        {
            uint32 itemId = combatConsumables[urand(0, combatConsumables.size() - 1)];
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
            if (!proto)
                continue;
            StoreItem(itemId, std::max<uint32>(1, std::min<uint32>(5, proto->GetMaxStackSize())));
        }
    }

    if (!bandages.empty())
    {
        uint32 itemId = bandages[urand(0, bandages.size() - 1)];
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
        if (proto)
            StoreItem(itemId, std::max<uint32>(1, std::min<uint32>(20, proto->GetMaxStackSize())));
    }
}

static std::vector<uint32> CollectPotionsForEffect(Player* bot, uint32 effectId)
{
    std::vector<uint32> items;
    ItemTemplateContainer const* itemTemplates = sObjectMgr->GetItemTemplateStore();
    for (auto const& itr : *itemTemplates)
    {
        ItemTemplate const* proto = &itr.second;
        if (!proto)
            continue;
        if (proto->Class != ITEM_CLASS_CONSUMABLE ||
            (proto->SubClass != ITEM_SUBCLASS_POTION && proto->SubClass != ITEM_SUBCLASS_FLASK))
            continue;
        bool matches = false;
        for (uint8 j = 0; j < MAX_ITEM_PROTO_SPELLS && !matches; ++j)
        {
            uint32 spellId = proto->Spells[j].SpellId;
            if (!spellId || proto->Spells[j].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
                continue;
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (!spellInfo)
                continue;
            for (uint8 i = 0; i < 3; ++i)
            {
                if (spellInfo->Effects[i].Effect == effectId)
                {
                    matches = true;
                    break;
                }
            }
        }
        if (!matches)
            continue;
        if (proto->RequiredLevel > bot->GetLevel())
            continue;
        items.push_back(proto->ItemId);
    }
    return items;
}

void BotFactory::InitPotions()
{
    uint32 effects[] = { SPELL_EFFECT_HEAL, SPELL_EFFECT_ENERGIZE };
    for (uint32 effect : effects)
    {
        if (effect == SPELL_EFFECT_ENERGIZE && bot->GetPowerType() != POWER_MANA)
            continue;

        FindPotionVisitor visitor(bot, effect);
        UseItemAction::IterateItems(bot, visitor, ITERATE_ITEMS_IN_BAGS);
        if (!visitor.GetResult().empty())
            continue;

        std::vector<uint32> potions = CollectPotionsForEffect(bot, effect);
        if (potions.empty())
            continue;
        uint32 itemId = potions[urand(0, potions.size() - 1)];
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
        if (!proto)
            continue;
        uint32 maxCount = proto->GetMaxStackSize();
        StoreItem(itemId, urand(std::max<uint32>(1, maxCount / 2), std::max<uint32>(1, maxCount)));
    }
}

void BotFactory::InitFood()
{
    if (botAI && botAI->HasCheat(BotCheatMask::food))
        return;

    std::unordered_map<uint32, std::vector<uint32>> items;
    ItemTemplateContainer const* itemTemplateContainer = sObjectMgr->GetItemTemplateStore();
    for (ItemTemplateContainer::const_iterator i = itemTemplateContainer->begin(); i != itemTemplateContainer->end(); ++i)
    {
        ItemTemplate const* proto = &i->second;
        if (!proto)
            continue;
        if (proto->Class != ITEM_CLASS_CONSUMABLE ||
            (proto->SubClass != ITEM_SUBCLASS_CONSUMABLE && proto->SubClass != ITEM_SUBCLASS_FOOD_DRINK))
            continue;
        if (proto->Spells[0].SpellCategory != 11 && proto->Spells[0].SpellCategory != 59)
            continue;
        if (proto->Bonding != BIND_NONE)
            continue;
        if (proto->RequiredLevel > bot->GetLevel() || proto->RequiredLevel < bot->GetLevel() - 9)
            continue;
        if (proto->RequiredSkill && !bot->HasSkill(proto->RequiredSkill))
            continue;
        if (proto->Area || proto->Map || proto->RequiredCityRank || proto->RequiredHonorRank)
            continue;
        items[proto->Spells[0].SpellCategory].push_back(proto->ItemId);
    }

    uint32 categories[] = { 11, 59 };
    for (uint32 category : categories)
    {
        std::vector<uint32>& ids = items[category];
        if (ids.empty())
            continue;
        for (int j = 0; j < 2; j++)
        {
            uint32 index = urand(0, ids.size() - 1);
            uint32 itemId = ids[index];
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
            if (!proto)
                continue;
            StoreItem(itemId, proto->GetMaxStackSize());
        }
    }
}

void BotFactory::InitMounts()
{
    uint32 firstmount = sPlayerbotAIConfig->useGroundMountAtMinLevel;
    uint32 secondmount = sPlayerbotAIConfig->useFastGroundMountAtMinLevel;
    uint32 thirdmount = sPlayerbotAIConfig->useFlyMountAtMinLevel;
    uint32 fourthmount = sPlayerbotAIConfig->useFastFlyMountAtMinLevel;

    if (bot->GetLevel() < firstmount)
        return;

    std::vector<uint32> slow, fast, fslow, ffast;
    switch (bot->GetRace())
    {
        case RACE_HUMAN: slow = {470, 6648, 458, 472}; fast = {23228, 23227, 23229}; break;
        case RACE_ORC: slow = {6654, 6653, 580}; fast = {23250, 23252, 23251}; break;
        case RACE_DWARF: slow = {6899, 6777, 6898}; fast = {23238, 23239, 23240}; break;
        case RACE_NIGHTELF: slow = {10789, 8394, 10793}; fast = {23219, 23220, 63637}; break;
        case RACE_UNDEAD_PLAYER: slow = {17463, 17464, 17462}; fast = {17465, 23246, 66846}; break;
        case RACE_TAUREN: slow = {18990, 18989, 64657}; fast = {23249, 23248, 23247}; break;
        case RACE_GNOME: slow = {10969, 17453, 10873, 17454}; fast = {23225, 23223, 23222}; break;
        case RACE_TROLL: slow = {10796, 10799, 8395}; fast = {23241, 23242, 23243}; break;
        case RACE_DRAENEI: slow = {34406, 35711, 35710}; fast = {35713, 35712, 35714}; break;
        case RACE_BLOODELF: slow = {33660, 35020, 35022, 35018}; fast = {35025, 35025, 35027}; break;
        default: break;
    }

    switch (bot->GetTeamId())
    {
        case TEAM_ALLIANCE:
            fslow = {32235, 32239, 32240};
            ffast = {32242, 32289, 32290, 32292};
            break;
        case TEAM_HORDE:
            fslow = {32244, 32245, 32243};
            ffast = {32295, 32297, 32246, 32296};
            break;
        default:
            break;
    }

    std::vector<uint32> types[4] = { slow, fast, fslow, ffast };
    for (uint32 type = 0; type < 4; ++type)
    {
        std::vector<uint32>& list = types[type];
        if (list.empty())
            continue;

        bool hasMount = false;
        for (uint32 spell : list)
        {
            if (bot->HasSpell(spell))
            {
                hasMount = true;
                break;
            }
        }
        if (hasMount)
            continue;

        if (bot->GetLevel() < secondmount && type == 1)
            continue;
        if (bot->GetLevel() < thirdmount && type == 2)
            continue;
        if (bot->GetLevel() < fourthmount && type == 3)
            continue;

        uint32 spell = list[urand(0, list.size() - 1)];
        if (spell)
            bot->LearnSpell(spell, false);
    }
}

void BotFactory::InitBags(bool destroyOld)
{
    std::vector<uint32> bagItems;
    ItemTemplateContainer const* itemTemplateContainer = sObjectMgr->GetItemTemplateStore();
    for (auto const& itr : *itemTemplateContainer)
    {
        ItemTemplate const* proto = &itr.second;
        if (!proto)
            continue;
        if (proto->Class != ITEM_CLASS_CONTAINER)
            continue;
        if (proto->RequiredLevel > bot->GetLevel())
            continue;
        if (proto->Bonding == BIND_QUEST)
            continue;
        if (proto->ContainerSlots < 4)
            continue;
        bagItems.push_back(proto->ItemId);
    }

    if (bagItems.empty())
        return;

    for (uint8 slot = INVENTORY_SLOT_BAG_START; slot < INVENTORY_SLOT_BAG_END; ++slot)
    {
        Item* oldBag = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (oldBag && !destroyOld)
            continue;
        if (oldBag)
            bot->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);

        uint32 bestBag = 0;
        uint32 bestSize = 0;
        for (uint32 itemId : bagItems)
        {
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
            if (!proto)
                continue;
            if (proto->ContainerSlots > bestSize)
            {
                bestSize = proto->ContainerSlots;
                bestBag = proto->ItemId;
            }
        }
        if (!bestBag)
            continue;
        uint16 dest = (INVENTORY_SLOT_BAG_0 << 8) | slot;
        bot->EquipNewItem(dest, bestBag, true);
    }
}

void BotFactory::InitGlyphs(bool /*increment*/)
{
    bot->InitGlyphsForLevel();

    SpellFamilyNames family = SPELLFAMILY_GENERIC;
    switch (bot->GetClass())
    {
        case CLASS_WARRIOR: family = SPELLFAMILY_WARRIOR; break;
        case CLASS_PALADIN: family = SPELLFAMILY_PALADIN; break;
        case CLASS_HUNTER: family = SPELLFAMILY_HUNTER; break;
        case CLASS_ROGUE: family = SPELLFAMILY_ROGUE; break;
        case CLASS_PRIEST: family = SPELLFAMILY_PRIEST; break;
        case CLASS_DEATH_KNIGHT: family = SPELLFAMILY_DEATHKNIGHT; break;
        case CLASS_SHAMAN: family = SPELLFAMILY_SHAMAN; break;
        case CLASS_MAGE: family = SPELLFAMILY_MAGE; break;
        case CLASS_WARLOCK: family = SPELLFAMILY_WARLOCK; break;
        case CLASS_DRUID: family = SPELLFAMILY_DRUID; break;
        case CLASS_MONK: family = SPELLFAMILY_MONK; break;
        default: break;
    }

    std::unordered_map<uint32, std::vector<uint32>> glyphsByType;
    for (uint32 i = 0; i < sGlyphPropertiesStore.GetNumRows(); ++i)
    {
        GlyphPropertiesEntry const* gp = sGlyphPropertiesStore.LookupEntry(i);
        if (!gp)
            continue;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(gp->SpellId);
        if (!spellInfo)
            continue;
        if (spellInfo->SpellFamilyName != family)
            continue;
        glyphsByType[gp->TypeFlags].push_back(gp->Id);
    }

    uint32 slotMask = bot->GetUInt32Value(PLAYER_FIELD_GLYPH_SLOTS_ENABLED);
    std::unordered_set<uint32> used;
    for (uint8 slot = 0; slot < MAX_GLYPH_SLOT_INDEX; ++slot)
    {
        if (!(slotMask & (1u << slot)))
            continue;

        uint32 slotEntryId = bot->GetGlyphSlot(slot);
        GlyphSlotEntry const* slotEntry = sGlyphSlotStore.LookupEntry(slotEntryId);
        if (!slotEntry)
            continue;

        std::vector<uint32> candidates;
        for (auto const& kvp : glyphsByType)
        {
            if ((kvp.first & slotEntry->TypeFlags) == 0)
                continue;
            for (uint32 glyphId : kvp.second)
            {
                if (used.find(glyphId) == used.end())
                    candidates.push_back(glyphId);
            }
        }

        if (candidates.empty())
            continue;

        uint32 glyphId = candidates[urand(0, candidates.size() - 1)];
        bot->SetGlyph(slot, glyphId);
        used.insert(glyphId);
    }

    bot->SendTalentsInfoData();
}

bool BotFactory::AreEnchantCachesLoaded()
{
    return s_enchantCachesLoaded;
}

void BotFactory::LoadEnchantCaches()
{
    if (s_enchantCachesLoaded)
        return;

    uint32 maxStoreSize = sSpellMgr->GetSpellInfoStoreSize();
    for (uint32 id = 1; id < maxStoreSize; ++id)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(id);
        if (!spellInfo)
            continue;
        if (strstr(spellInfo->SpellName[0], "Test"))
            continue;
        for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            if (spellInfo->Effects[j].Effect != SPELL_EFFECT_ENCHANT_ITEM)
                continue;
            uint32 enchant_id = spellInfo->Effects[j].MiscValue;
            if (!enchant_id)
                continue;
        SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!enchant)
            continue;
            s_enchantSpellIdCache.push_back(id);
            break;
        }
    }

    for (auto iter = sSpellItemEnchantmentStore.begin(); iter != sSpellItemEnchantmentStore.end(); ++iter)
    {
        uint32 gemId = iter->GemID;
        if (gemId == 0)
            continue;
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(gemId);
        if (!proto || !sGemPropertiesStore.LookupEntry(proto->GemProperties))
            continue;
        if (proto->ItemLevel < 60)
            continue;
        s_enchantGemIdCache.push_back(gemId);
    }

    s_enchantCachesLoaded = true;
}

std::vector<uint32> BotFactory::GetCurrentGemsCount()
{
    std::vector<uint32> curcount(4);
    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        Item* item = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (!item || item->IsBroken())
            continue;
        ItemTemplate const* itemProto = item->GetTemplate();
        if (!itemProto ||
            (itemProto->Socket[0].Color == 0 && itemProto->Socket[1].Color == 0 && itemProto->Socket[2].Color == 0))
            continue;
        for (uint32 enchant_slot = SOCK_ENCHANTMENT_SLOT; enchant_slot <= PRISMATIC_ENCHANTMENT_SLOT; ++enchant_slot)
        {
            if (enchant_slot == BONUS_ENCHANTMENT_SLOT)
                continue;
            uint32 enchant_id = item->GetEnchantmentId(EnchantmentSlot(enchant_slot));
            if (!enchant_id)
                continue;
            SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
            if (!enchantEntry)
                continue;
            uint32 gemid = enchantEntry->GemID;
            if (!gemid)
                continue;
            ItemTemplate const* gemProto = sObjectMgr->GetItemTemplate(gemid);
            if (!gemProto)
                continue;
            GemPropertiesEntry const* gemProperty = sGemPropertiesStore.LookupEntry(gemProto->GemProperties);
            if (!gemProperty)
                continue;
            uint8 gemColor = gemProperty->color;
            for (uint8 b = 0, tmpcolormask = 1; b < 4; b++, tmpcolormask <<= 1)
            {
                if (tmpcolormask & gemColor)
                    ++curcount[b];
            }
        }
    }
    return curcount;
}

void BotFactory::ApplyEnchantAndGemsNew(bool /*destroyOld*/)
{
    LoadEnchantCaches();

    for (uint8 slot = 0; slot < EQUIPMENT_SLOT_END; ++slot)
    {
        if (slot == EQUIPMENT_SLOT_TABARD || slot == EQUIPMENT_SLOT_BODY)
            continue;
        Item* item = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!item || !item->GetOwner())
            continue;
        if (item->GetTemplate() && item->GetTemplate()->Quality < ITEM_QUALITY_UNCOMMON)
            continue;

        int32 bestEnchantId = -1;
        for (const uint32 enchantSpell : s_enchantSpellIdCache)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(enchantSpell);
            if (!spellInfo)
                continue;
            if (!item->IsFitToSpellRequirements(spellInfo))
                continue;
            uint32 RequiredLevel = spellInfo->BaseLevel;
            if (RequiredLevel > bot->GetLevel())
                continue;
            for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
            {
                if (spellInfo->Effects[j].Effect != SPELL_EFFECT_ENCHANT_ITEM)
                    continue;
                uint32 enchant_id = spellInfo->Effects[j].MiscValue;
                if (!enchant_id)
                    continue;
                SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                if (!enchant)
                    continue;
                if (enchant->RequiredSkill &&
                    (!bot->HasSkill(enchant->RequiredSkill) ||
                        (bot->GetSkillValue(enchant->RequiredSkill) < enchant->RequiredSkillValue)))
                    continue;
                if (enchant->RequiredLevel > bot->GetLevel())
                    continue;
                bestEnchantId = enchant_id;
                break;
            }
            if (bestEnchantId != -1)
                break;
        }

        if (bestEnchantId != -1)
        {
            bot->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, false);
            item->SetEnchantment(PERM_ENCHANTMENT_SLOT, bestEnchantId, 0, 0, bot->GetGUID());
            bot->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, true);
        }

        ItemTemplate const* itemProto = item->GetTemplate();
        if (!itemProto ||
            (itemProto->Socket[0].Color == 0 && itemProto->Socket[1].Color == 0 && itemProto->Socket[2].Color == 0))
            continue;

        for (uint32 enchant_slot = SOCK_ENCHANTMENT_SLOT; enchant_slot < SOCK_ENCHANTMENT_SLOT + 3; ++enchant_slot)
        {
            uint8 socketColor = itemProto->Socket[enchant_slot - SOCK_ENCHANTMENT_SLOT].Color;
            if (!socketColor)
                continue;
            int32 enchantIdChosen = -1;
            for (uint32 enchantGem : s_enchantGemIdCache)
            {
                ItemTemplate const* gemTemplate = sObjectMgr->GetItemTemplate(enchantGem);
                if (!gemTemplate)
                    continue;
                GemPropertiesEntry const* gemProperties = sGemPropertiesStore.LookupEntry(gemTemplate->GemProperties);
                if (!gemProperties)
                    continue;
                if ((socketColor & gemProperties->color) == 0 && gemProperties->color == 1)
                    continue;
                uint32 enchant_id = gemProperties->spellitemenchantement;
                if (!enchant_id)
                    continue;
                enchantIdChosen = enchant_id;
                break;
            }
            if (enchantIdChosen == -1)
                continue;
            bot->ApplyEnchantment(item, EnchantmentSlot(enchant_slot), false);
            item->SetEnchantment(EnchantmentSlot(enchant_slot), enchantIdChosen, 0, 0, bot->GetGUID());
            bot->ApplyEnchantment(item, EnchantmentSlot(enchant_slot), true);
        }
    }
}

void BotFactory::InitPetTalents()
{
    // MoP: keep minimal for now.
}

void BotFactory::InitReputation()
{
    // MoP: skip TBC/WotLK heroic key reps.
}

Item* BotFactory::StoreItem(uint32 itemId, uint32 count)
{
    if (!itemId || !count)
        return nullptr;
    ItemPosCountVec dest;
    InventoryResult res = bot->CanStoreNewItem(INVENTORY_SLOT_BAG_0, NULL_SLOT, dest, itemId, count);
    if (res != EQUIP_ERR_OK)
        return nullptr;
    Item* item = bot->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));
    if (!item)
        return nullptr;
    item->AddToUpdateQueueOf(bot);
    return item;
}

void BotFactory::ClearEverything()
{
    bot->GiveLevel(bot->GetClass() == CLASS_DEATH_KNIGHT ? sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)
                                                        : sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL));
    bot->SetUInt32Value(PLAYER_FIELD_XP, 0);
    TC_LOG_INFO("playerbots", "Resetting player...");
    bot->ResetTalents(true);
}
  
ObjectGuid BotFactory::GetRandomBot()
{
    GuidVector guids;
    for (std::vector<uint32>::iterator i = sPlayerbotAIConfig->randomBotAccounts.begin();
        i != sPlayerbotAIConfig->randomBotAccounts.end(); i++)
    {
        uint32 accountId = *i;
        if (!AccountMgr::GetCharactersCount(accountId))
            continue;
 
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARS_BY_ACCOUNT_ID);
        stmt->setUInt32(0, accountId);
        PreparedQueryResult result = CharacterDatabase.Query(stmt);
        if (!result)
            continue;
 
        do
        {
            Field* fields = result->Fetch();
            ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(fields[0].GetUInt32());
            if (!ObjectAccessor::FindPlayer(guid))
                guids.push_back(guid);
        } while (result->NextRow());
    }
 
    if (guids.empty())
        return ObjectGuid::Empty;
 
    uint32 index = urand(0, guids.size() - 1);
    return guids[index];
}
 
std::vector<InventoryType> BotFactory::GetPossibleInventoryTypeListBySlot(EquipmentSlots slot)
{
    std::vector<InventoryType> ret;
    switch (slot)
    {
    case EQUIPMENT_SLOT_HEAD:
        ret.push_back(INVTYPE_HEAD);
        break;
    case EQUIPMENT_SLOT_NECK:
        ret.push_back(INVTYPE_NECK);
        break;
    case EQUIPMENT_SLOT_SHOULDERS:
        ret.push_back(INVTYPE_SHOULDERS);
        break;
    case EQUIPMENT_SLOT_BODY:
        ret.push_back(INVTYPE_BODY);
        break;
    case EQUIPMENT_SLOT_CHEST:
        ret.push_back(INVTYPE_CHEST);
        ret.push_back(INVTYPE_ROBE);
        break;
    case EQUIPMENT_SLOT_WAIST:
        ret.push_back(INVTYPE_WAIST);
        break;
    case EQUIPMENT_SLOT_LEGS:
        ret.push_back(INVTYPE_LEGS);
        break;
    case EQUIPMENT_SLOT_FEET:
        ret.push_back(INVTYPE_FEET);
        break;
    case EQUIPMENT_SLOT_WRISTS:
        ret.push_back(INVTYPE_WRISTS);
        break;
    case EQUIPMENT_SLOT_HANDS:
        ret.push_back(INVTYPE_HANDS);
        break;
    case EQUIPMENT_SLOT_FINGER1:
    case EQUIPMENT_SLOT_FINGER2:
        ret.push_back(INVTYPE_FINGER);
        break;
    case EQUIPMENT_SLOT_TRINKET1:
    case EQUIPMENT_SLOT_TRINKET2:
        ret.push_back(INVTYPE_TRINKET);
        break;
    case EQUIPMENT_SLOT_BACK:
        ret.push_back(INVTYPE_CLOAK);
        break;
    case EQUIPMENT_SLOT_MAINHAND:
        ret.push_back(INVTYPE_WEAPON);
        ret.push_back(INVTYPE_2HWEAPON);
        ret.push_back(INVTYPE_WEAPONMAINHAND);
        ret.push_back(INVTYPE_RANGED);
        break;
    case EQUIPMENT_SLOT_OFFHAND:
        ret.push_back(INVTYPE_WEAPON);
        ret.push_back(INVTYPE_2HWEAPON);
        ret.push_back(INVTYPE_WEAPONOFFHAND);
        ret.push_back(INVTYPE_SHIELD);
        ret.push_back(INVTYPE_HOLDABLE);
        break;
    case EQUIPMENT_SLOT_RANGED:
        ret.push_back(INVTYPE_RANGED);
        break;
    default:
        break;
    }
    return ret;
}

bool BotFactory::CanEquipUnseenItem(uint8 slot, uint16& dest, uint32 item)
{
    dest = 0;

    if (Item* pItem = Item::CreateItem(item, 1, bot, true))
    {
        InventoryResult result = botAI ? botAI->CanEquipItem(slot, dest, pItem, true, true)
            : bot->CanEquipItem(slot, dest, pItem, true, true);
        pItem->RemoveFromUpdateQueueOf(bot);
        delete pItem;
        return result == EQUIP_ERR_OK;
    }

    return false;
}

bool BotFactory::CanEquipItem(ItemTemplate const* proto)
{
    if (proto->Duration != 0)
        return false;

    if (proto->Bonding == BIND_QUEST /*|| proto->Bonding == BIND_WHEN_USE*/)
        return false;

    if (proto->Class == ITEM_CLASS_CONTAINER)
        return true;

    uint32 RequiredLevel = proto->RequiredLevel;
    bool hasItem = bot->HasItemCount(proto->ItemId, 1, false);
    if (!RequiredLevel && hasItem)
        return false;

    uint32 level = bot->GetLevel();

    if (RequiredLevel > level)
        return false;

    return true;
}

void BotFactory::InitEquipment(bool incremental, bool second_chance)
{
    std::unordered_map<uint8, std::vector<uint32>> items;
    uint32 blevel = bot->GetLevel();
    int32 delta = std::min(blevel, 10u);

    for (int32 slot = (int32)EQUIPMENT_SLOT_TABARD; slot >= (int32)EQUIPMENT_SLOT_START; slot--)
    {
        if (slot == EQUIPMENT_SLOT_TABARD || slot == EQUIPMENT_SLOT_BODY || slot == EQUIPMENT_SLOT_RANGED)
            continue;

        if (level < 50 && (slot == EQUIPMENT_SLOT_TRINKET1 || slot == EQUIPMENT_SLOT_TRINKET2))
            continue;

        if (level < 30 && slot == EQUIPMENT_SLOT_NECK)
            continue;

        if (level < 25 && slot == EQUIPMENT_SLOT_HEAD)
            continue;

        if (level < 20 && (slot == EQUIPMENT_SLOT_FINGER1 || slot == EQUIPMENT_SLOT_FINGER2))
            continue;

        Item* oldItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (oldItem && second_chance)
        {
            bot->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
        }

        oldItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        bool isforcedbreak = false;
        int maxiRetry = 10;
        do
        {
            for (InventoryType inventoryType : GetPossibleInventoryTypeListBySlot((EquipmentSlots)slot))
            {
                uint32 itemid = sRandomItemMgr->FindBestItemForLevelAndEquip(bot, inventoryType);
                if (itemid)
                {
                    uint32 skipProb = 25;
                    if (urand(1, 100) <= skipProb)
                        continue;

                    items[slot].push_back(itemid);
                }
                else
                {
                    maxiRetry--;
                    if (maxiRetry <= 0)
                    {
                        isforcedbreak = true;
                        break;
                    }
                }
            }
        } while (items[slot].size() < 25 && !isforcedbreak);

        std::vector<uint32>& ids = items[slot];
        if (ids.empty())
        {
            continue;
        }

        uint32 bestItemForSlot = 0;
        for (int index = 0; index < ids.size(); index++)
        {
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(ids[index]);

            // delay heavy check to here
            if (!CanEquipItem(proto))
                continue;
            uint16 dest;
            if (!CanEquipUnseenItem(slot, dest, proto->ItemId))
                continue;
            bestItemForSlot = proto->ItemId;
        }

        if (bestItemForSlot == 0)
        {
            continue;
        }
        uint16 dest;
        if (!CanEquipUnseenItem(slot, dest, bestItemForSlot))
        {
            continue;
        }

        oldItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        // fail to store in bag
        if (oldItem)
        {
            bot->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
            //continue;
        }
        Item* newItem = bot->EquipNewItem(dest, bestItemForSlot, true);
        bot->AutoUnequipOffhandIfNeed();
    }

    // Secondary init for better equips
    /// @todo: clean up duplicate code
    if (second_chance)
    {
        for (int32 slot = (int32)EQUIPMENT_SLOT_TABARD; slot >= (int32)EQUIPMENT_SLOT_START; slot--)
        {
            if (slot == EQUIPMENT_SLOT_TABARD || slot == EQUIPMENT_SLOT_BODY)
                continue;

            if (level < 50 && (slot == EQUIPMENT_SLOT_TRINKET1 || slot == EQUIPMENT_SLOT_TRINKET2))
                continue;

            if (level < 30 && slot == EQUIPMENT_SLOT_NECK)
                continue;

            if (level < 25 && slot == EQUIPMENT_SLOT_HEAD)
                continue;

            if (level < 20 && (slot == EQUIPMENT_SLOT_FINGER1 || slot == EQUIPMENT_SLOT_FINGER2))
                continue;

            if (Item* oldItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                bot->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);

            std::vector<uint32>& ids = items[slot];
            if (ids.empty())
                continue;

            float bestScoreForSlot = -1;
            uint32 bestItemForSlot = 0;
            for (int index = 0; index < ids.size(); index++)
            {
                ItemTemplate const* proto = sObjectMgr->GetItemTemplate(ids[index]);
                // delay heavy check to here
                if (!CanEquipItem(proto))
                    continue;
                uint16 dest;
                if (!CanEquipUnseenItem(slot, dest, proto->ItemId))
                    continue;
                bestItemForSlot = proto->ItemId;
            }

            if (bestItemForSlot == 0)
            {
                continue;
            }
            uint16 dest;
            if (!CanEquipUnseenItem(slot, dest, bestItemForSlot))
            {
                continue;
            }
            Item* newItem = bot->EquipNewItem(dest, bestItemForSlot, true);
            bot->AutoUnequipOffhandIfNeed();
        }
    }
}
