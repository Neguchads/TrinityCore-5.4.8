#include "SecurityCheckAction.h"

#include "Group.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"

bool SecurityCheckAction::isUseful()
{
    if (!bot || !botAI)
        return false;

    if (!sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    Player* master = botAI->GetMaster();
    if (!master || !master->GetSession())
        return false;

    // Only enforce for non-GM real players (don't block staff/self-bot setups).
    if (master->GetSession()->GetSecurity() >= SEC_GAMEMASTER)
        return false;

    // If master is a bot/controller, ignore.
    if (GET_PLAYERBOT_AI(master))
        return false;

    return true;
}

bool SecurityCheckAction::Execute(Event event)
{
    Player* requester = event.getOwner() ? event.getOwner() : botAI->GetMaster();

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    LootMethod method = group->GetLootMethod();
    ItemQualities threshold = group->GetLootThreshold();
    if (method == MASTER_LOOT || method == FREE_FOR_ALL || threshold > ITEM_QUALITY_UNCOMMON)
    {
        Player* groupMaster = botAI->GetGroupMaster();
        if (groupMaster && groupMaster->GetSession() && groupMaster->GetSession()->GetSecurity() == SEC_PLAYER)
        {
            bool guildOk = (bot->GetGuildId() && groupMaster->GetGuildId() && bot->GetGuildId() == groupMaster->GetGuildId());
            if (!guildOk)
            {
                if (requester)
                    botAI->TellError("I will play with this loot type only if I'm in your guild :/");

                botAI->ChangeStrategy("+passive,+stay", BOT_STATE_NON_COMBAT);
                botAI->ChangeStrategy("+passive,+stay", BOT_STATE_COMBAT);
            }
        }
        return true;
    }

    return false;
}
