#include "StartDuelAction.h"

#include "ObjectAccessor.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "SharedDefines.h"

namespace
{
    constexpr uint32 SPELL_DUEL = 7266;
    constexpr uint32 SPELL_MOUNTED_DUEL = 62875;
}

Player* StartDuelAction::SelectOpponent()
{
    if (!bot)
        return nullptr;

    ObjectGuid masterGuid;
    if (botAI && botAI->HasRealPlayerMaster() && botAI->GetMaster())
        masterGuid = botAI->GetMaster()->GetGUID();

    GuidVector players = AI_VALUE(GuidVector, "nearest friendly players");
    for (ObjectGuid const& guid : players)
    {
        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player)
            continue;
        if (player->IsInCombat() || player->duel)
            continue;
        if (player->IsGameMaster())
            continue;
        if (player->GetGroup() && bot->GetGroup() && player->GetGroup() == bot->GetGroup() && player->GetGUID() != masterGuid)
            continue;
        if (!bot->IsWithinDistInMap(player, INTERACTION_DISTANCE))
            continue;

        return player;
    }

    return nullptr;
}

bool StartDuelAction::isUseful()
{
    if (!bot || bot->IsInCombat() || bot->duel)
        return false;
    if (!AI_VALUE(bool, "can move around"))
        return false;

    return SelectOpponent() != nullptr;
}

bool StartDuelAction::Execute(Event event)
{
    Player* opponent = SelectOpponent();
    if (!opponent)
        return false;

    uint32 spellId = bot->IsMounted() ? SPELL_MOUNTED_DUEL : SPELL_DUEL;
    bot->CastSpell(opponent, spellId, false);
    return true;
}
