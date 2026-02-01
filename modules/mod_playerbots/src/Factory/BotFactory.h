#ifndef _PLAYERBOT_BOTFACTORY_H
#define _PLAYERBOT_BOTFACTORY_H

#include "Player.h"
#include "PlayerbotAI.h"

class Item;

class BotFactory
{
public:
    BotFactory(Player* bot, uint32 level, uint32 itemQuality = 0, uint32 gearScoreLimit = 0);

    static ObjectGuid GetRandomBot();
    static void Init();
    void Refresh();
    void Randomize(bool incremental);
    void ClearEverything();

    void InitSkills();
    void InitAvailableSpells();
    void InitClassSpells();
    void InitSpecialSpells();
    void InitReagents();
    void InitConsumables();
    void InitPotions();
    void InitFood();
    void InitMounts();
    void InitBags(bool destroyOld = true);
    void InitGlyphs(bool increment = false);
    void ApplyEnchantAndGemsNew(bool destroyOld = true);
    void InitPetTalents();
    void InitReputation();

    void InitEquipment(bool incremental, bool second_chance = false);
    void InitPet();
    void InitTalentsTree(bool reset);
private:
    void Prepare();
    void CancelAuras();
    void SetRandomSkill(uint16 id);
    Item* StoreItem(uint32 itemId, uint32 count);
    std::vector<uint32> GetCurrentGemsCount();
    static void LoadEnchantCaches();
    static bool AreEnchantCachesLoaded();

    // -- GEARING
    bool CanEquipItem(ItemTemplate const* proto);
    bool CanEquipUnseenItem(uint8 slot, uint16& dest, uint32 item);
    std::vector<InventoryType> GetPossibleInventoryTypeListBySlot(EquipmentSlots slot);

    uint32 level;
protected:
    Player* bot;
    PlayerbotAI* botAI;
};

#endif
