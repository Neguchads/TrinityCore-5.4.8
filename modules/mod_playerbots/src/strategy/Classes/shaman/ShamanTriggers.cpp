

#include "ShamanTriggers.h"

#include "Item.h"
#include "Player.h"
#include "Playerbots.h"

namespace
{
    bool IsWeaponWithoutImbue(Player* bot, uint8 slot)
    {
        Item* item = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!item || item->GetTemplate()->Class != ITEM_CLASS_WEAPON)
            return false;

        return item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == 0;
    }
}

bool TotemMissingTrigger::IsActive() { return !AI_VALUE2(bool, "has totem", totem); }

bool MainHandWeaponNoImbueTrigger::IsActive()
{
    return IsWeaponWithoutImbue(botAI->GetBot(), EQUIPMENT_SLOT_MAINHAND);
}

bool OffHandWeaponNoImbueTrigger::IsActive()
{
    return IsWeaponWithoutImbue(botAI->GetBot(), EQUIPMENT_SLOT_OFFHAND);
}

bool NoFireTotemTrigger::IsActive()
{
    return !AI_VALUE2(bool, "has totem", "searing totem") &&
           !AI_VALUE2(bool, "has totem", "magma totem") &&
           !AI_VALUE2(bool, "has totem", "flametongue totem") &&
           !AI_VALUE2(bool, "has totem", "totem of wrath") &&
           !AI_VALUE2(bool, "has totem", "fire elemental totem");
}

bool NoAirTotemTrigger::IsActive()
{
    return !AI_VALUE2(bool, "has totem", "windfury totem") &&
           !AI_VALUE2(bool, "has totem", "wrath of air totem") &&
           !AI_VALUE2(bool, "has totem", "grace of air totem") &&
           !AI_VALUE2(bool, "has totem", "windwalk totem") &&
           !AI_VALUE2(bool, "has totem", "stormlash totem");
}

bool NoWaterTotemTrigger::IsActive()
{
    return !AI_VALUE2(bool, "has totem", "mana spring totem") &&
           !AI_VALUE2(bool, "has totem", "healing stream totem") &&
           !AI_VALUE2(bool, "has totem", "mana tide totem") &&
           !AI_VALUE2(bool, "has totem", "healing tide totem") &&
           !AI_VALUE2(bool, "has totem", "cleansing totem");
}

bool NoEarthTotemTrigger::IsActive()
{
    return !AI_VALUE2(bool, "has totem", "strength of earth totem") &&
           !AI_VALUE2(bool, "has totem", "stoneskin totem") &&
           !AI_VALUE2(bool, "has totem", "earthbind totem") &&
           !AI_VALUE2(bool, "has totem", "stone bulwark totem") &&
           !AI_VALUE2(bool, "has totem", "earth elemental totem") &&
           !AI_VALUE2(bool, "has totem", "spirit link totem");
}
