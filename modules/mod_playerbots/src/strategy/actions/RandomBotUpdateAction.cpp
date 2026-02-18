#include "RandomBotUpdateAction.h"

#include "Event.h"
#include "Playerbots.h"

bool RandomBotUpdateAction::Execute(Event event)
{
    if (!sRandomPlayerbotMgr->IsRandomBot(bot))
        return false;

    if (bot->GetGroup() && botAI->GetGroupMaster())
    {
        PlayerbotAI* groupMasterBotAI = GET_PLAYERBOT_AI(botAI->GetGroupMaster());
        if (!groupMasterBotAI || groupMasterBotAI->IsRealPlayer())
            return true;
    }

    if (botAI->HasPlayerNearby(sPlayerbotAIConfig->grindDistance))
        return true;

    // Random bot lifecycle updates are processed by RandomPlayerbotMgr on the world thread.
    // Avoid calling ProcessBot from per-bot AI action context (map update threads).
    return true;
}

bool RandomBotUpdateAction::isUseful() { return AI_VALUE(bool, "random bot update"); }
