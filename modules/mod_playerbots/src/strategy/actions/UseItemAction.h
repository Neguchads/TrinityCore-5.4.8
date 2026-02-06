#ifndef _PLAYERBOT_USEITEMACTION_H
#define _PLAYERBOT_USEITEMACTION_H

#include "Action.h"
#include "ItemVisitors.h"

class Item;
class Player;
class PlayerbotAI;

class UseItemAction : public Action
{
public:
    UseItemAction(PlayerbotAI* botAI, std::string const name = "use", bool selfOnly = false)
        : Action(botAI, name), selfOnly(selfOnly)
    {
    }

    bool Execute(Event event) override;
    bool isPossible() override;
    static void IterateItems(Player* bot, IterateItemsVisitor& visitor, uint32 mask = ITERATE_ITEMS_IN_BAGS);
    static bool UseItemAuto(PlayerbotAI* botAI, Item* item, bool allowCombat = true);

protected:
    Item* FindFirstItemByName(std::string const& name) const;
    bool UseItemOnSelf(Item* item, bool allowCombat = true);

    bool selfOnly;
};

class UseHealingPotion : public UseItemAction
{
public:
    UseHealingPotion(PlayerbotAI* botAI) : UseItemAction(botAI, "healing potion") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class UseManaPotion : public UseItemAction
{
public:
    UseManaPotion(PlayerbotAI* botAI) : UseItemAction(botAI, "mana potion") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

#endif
