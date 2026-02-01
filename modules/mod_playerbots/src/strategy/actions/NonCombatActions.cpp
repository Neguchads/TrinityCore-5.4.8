/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#include "NonCombatActions.h"

#include <algorithm>

#include "Event.h"
#include "Item.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"

namespace
{
Item* FindConsumable(Player* bot, uint32 spellCategory)
{
    FindFoodVisitor visitor(bot, spellCategory);
    UseItemAction::IterateItems(bot, visitor, ITERATE_ITEMS_IN_BAGS);

    std::vector<Item*>& items = visitor.GetResult();
    if (items.empty())
        return nullptr;

    return items.front();
}

void SetConsumableDelay(PlayerbotAI* botAI, Player* bot, Item* item)
{
    if (!botAI || !bot || !item)
        return;

    ItemTemplate const* proto = item->GetTemplate();
    if (!proto)
        return;

    bool isDrink = proto->Spells[0].SpellCategory == 59;
    bool isFood = proto->Spells[0].SpellCategory == 11;

    float hp = bot->GetHealthPct();
    float mp = bot->GetMaxPower(POWER_MANA) > 0 ? (bot->GetPower(POWER_MANA) * 100.0f / bot->GetMaxPower(POWER_MANA))
                                                : 100.0f;
    float p = 0.0f;

    if (isDrink && isFood)
        p = std::min(hp, mp);
    else if (isDrink)
        p = mp;
    else if (isFood)
        p = std::min(hp, mp);

    float delay = 27000.0f * (100.0f - p) / 100.0f;
    botAI->SetNextCheckDelay(std::max(10000.0f, delay));
}
}

bool DrinkAction::Execute(Event /*event*/)
{
    if (botAI->HasCheat(BotCheatMask::food))
    {
        bot->ClearUnitState(UNIT_STATE_CHASE);
        bot->ClearUnitState(UNIT_STATE_FOLLOW);

        if (bot->isMoving())
            bot->StopMoving();

        bot->SetStandState(UNIT_STAND_STATE_SIT);
        botAI->InterruptSpell();

        float mp = bot->GetPowerPct(POWER_MANA);
        float delay = (!bot->InBattleground() ? 18000.0f : 12000.0f) * (100.0f - mp) / 100.0f;
        botAI->SetNextCheckDelay(delay);
        bot->AddAura(25990, bot);
        return true;
    }

    if (bot->IsInCombat())
        return false;

    Item* drink = FindConsumable(bot, 59);
    if (!drink)
        return false;

    if (!UseItemOnSelf(drink, false))
        return false;

    SetConsumableDelay(botAI, bot, drink);
    return true;
}

bool DrinkAction::isUseful()
{
    return UseItemAction::isUseful() &&
        AI_VALUE2(bool, "has mana", "self target") &&
        AI_VALUE2(uint8, "mana", "self target") < 100;
}

bool DrinkAction::isPossible()
{
    return !bot->IsInCombat() &&
        !bot->IsMounted() &&
        !botAI->HasAnyAuraOf(GetTarget(), "dire bear form", "bear form", "cat form", "travel form",
            "aquatic form", "flight form", "swift flight form", nullptr) &&
        (botAI->HasCheat(BotCheatMask::food) || UseItemAction::isPossible());
}

bool EatAction::Execute(Event /*event*/)
{
    if (botAI->HasCheat(BotCheatMask::food))
    {
        bot->ClearUnitState(UNIT_STATE_CHASE);
        bot->ClearUnitState(UNIT_STATE_FOLLOW);

        if (bot->isMoving())
            bot->StopMoving();

        bot->SetStandState(UNIT_STAND_STATE_SIT);
        botAI->InterruptSpell();

        float hp = bot->GetHealthPct();
        float delay = (!bot->InBattleground() ? 18000.0f : 12000.0f) * (100.0f - hp) / 100.0f;
        botAI->SetNextCheckDelay(delay);
        bot->AddAura(25990, bot);
        return true;
    }

    if (bot->IsInCombat())
        return false;

    Item* food = FindConsumable(bot, 11);
    if (!food)
        return false;

    if (!UseItemOnSelf(food, false))
        return false;

    SetConsumableDelay(botAI, bot, food);
    return true;
}

bool EatAction::isUseful()
{
    return UseItemAction::isUseful() &&
        AI_VALUE2(uint8, "health", "self target") < 100;
}

bool EatAction::isPossible()
{
    return !bot->IsInCombat() &&
        !bot->IsMounted() &&
        !botAI->HasAnyAuraOf(GetTarget(), "dire bear form", "bear form", "cat form", "travel form",
            "aquatic form", "flight form", "swift flight form", nullptr) &&
        (botAI->HasCheat(BotCheatMask::food) || UseItemAction::isPossible());
}
