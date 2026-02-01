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

#include "ChatActions.h"

#include <map>

#include "Common.h"
#include "DBCStores.h"
#include "Group.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotSpec.h"
#include "PlayerbotTextMgr.h"
#include "Playerbots.h"
#include "World.h"

namespace
{
std::string GetAreaName(uint32 areaId, uint8 locale)
{
    if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(areaId))
        return area->area_name[locale];

    return "";
}

std::map<std::string, std::string> BuildCommonPlaceholders(PlayerbotAI* botAI, Unit* target)
{
    std::map<std::string, std::string> placeholders;
    Player* bot = botAI->GetBot();
    if (!bot)
        return placeholders;

    if (target)
        placeholders["<target>"] = target->GetName();

    placeholders["<randomfaction>"] = bot->GetTeamId() == TEAM_ALLIANCE ? "Alliance" : "Horde";

    if (Item* const rangedItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
    {
        switch (rangedItem->GetTemplate()->SubClass)
        {
        case ITEM_SUBCLASS_WEAPON_GUN:
            placeholders["<ammo>"] = "bullets";
            break;
        case ITEM_SUBCLASS_WEAPON_BOW:
        case ITEM_SUBCLASS_WEAPON_CROSSBOW:
            placeholders["<ammo>"] = "arrows";
            break;
        default:
            break;
        }
    }

    uint8 locale = sWorld->GetDefaultDbcLocale();
    if (locale >= MAX_LOCALES)
        locale = 0;

    placeholders["%my_level"] = std::to_string(bot->GetLevel());
    placeholders["%my_class"] = GetClassName(bot->GetClass(), locale);
    placeholders["%my_race"] = GetRaceName(bot->GetRace(), locale);
    placeholders["%zone_name"] = GetAreaName(bot->GetZoneId(), locale);
    placeholders["%area_name"] = GetAreaName(bot->GetAreaId(), locale);

    if (PlayerBotSpec::IsTank(bot))
        placeholders["%my_role"] = "tank";
    else if (PlayerBotSpec::IsHeal(bot))
        placeholders["%my_role"] = "healer";
    else
        placeholders["%my_role"] = "dps";

    return placeholders;
}

bool SayText(Player* bot, std::string const& text)
{
    if (text.empty())
        return false;

    if (text.find("/y ") == 0)
    {
        bot->Yell(text.substr(3), bot->GetTeamId() == TEAM_ALLIANCE ? LANG_COMMON : LANG_ORCISH);
        return true;
    }

    bot->Say(text, bot->GetTeamId() == TEAM_ALLIANCE ? LANG_COMMON : LANG_ORCISH);
    return true;
}
}

SayAction::SayAction(PlayerbotAI* botAI) : Action(botAI, "say"), Qualified() {}

bool SayAction::Execute(Event /*event*/)
{
    std::string text;
    Unit* target = AI_VALUE(Unit*, "tank target");
    if (!target)
        target = AI_VALUE(Unit*, "current target");

    std::map<std::string, std::string> placeholders = BuildCommonPlaceholders(botAI, target);

    std::string textKey = qualifier.empty() ? "hello" : qualifier;
    std::string lastSaidKey = textKey;
    uint32 nextTime = time(nullptr) + urand(1, 30);
    botAI->GetAiObjectContext()->GetValue<time_t>("last said", lastSaidKey)->Set(nextTime);

    Group* group = bot->GetGroup();
    if (group)
    {
        std::vector<Player*> members;
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            PlayerbotAI* memberAi = GET_PLAYERBOT_AI(member);
            if (memberAi)
                members.push_back(member);
        }

        uint32 count = members.size();
        if (count > 1)
        {
            for (uint32 i = 0; i < count * 5; i++)
            {
                int i1 = urand(0, count - 1);
                int i2 = urand(0, count - 1);
                Player* item = members[i1];
                members[i1] = members[i2];
                members[i2] = item;
            }
        }

        int index = 0;
        for (auto& member : members)
        {
            PlayerbotAI* memberAi = GET_PLAYERBOT_AI(member);
            if (memberAi)
                memberAi->GetAiObjectContext()->GetValue<time_t>("last said", lastSaidKey)->Set(nextTime + (20 * ++index) + urand(1, 15));
        }
    }

    if (!sPlayerbotTextMgr->GetBotText(textKey, text, placeholders))
        return false;

    return SayText(bot, text);
}

bool SayAction::isUseful()
{
    if (!botAI->AllowActivity())
        return false;

    std::string lastSaidKey = qualifier.empty() ? "hello" : qualifier;
    time_t lastSaid = AI_VALUE2(time_t, "last said", lastSaidKey);
    return (time(nullptr) - lastSaid) > 30;
}

TalkAction::TalkAction(PlayerbotAI* botAI) : Action(botAI, "talk") {}

bool TalkAction::Execute(Event /*event*/)
{
    if (urand(1, 100) > 25)
        return false;

    std::map<std::string, std::string> placeholders = BuildCommonPlaceholders(botAI, nullptr);
    std::string text;

    if (!sPlayerbotTextMgr->GetBotText("suggest_something", text, placeholders))
        sPlayerbotTextMgr->GetBotText("hello", text, placeholders);

    if (!SayText(bot, text))
        return false;

    botAI->GetAiObjectContext()->GetValue<time_t>("last said", "chat")->Set(time(nullptr) + urand(5, 60));
    return true;
}

bool TalkAction::isUseful()
{
    if (!botAI->AllowActivity())
        return false;

    time_t lastSaid = AI_VALUE2(time_t, "last said", "chat");
    return (time(nullptr) - lastSaid) > 30;
}

EmoteAction::EmoteAction(PlayerbotAI* botAI) : Action(botAI, "emote") {}

bool EmoteAction::Execute(Event /*event*/)
{
    if (urand(1, 100) > 35)
        return false;

    static const uint32 emotes[] = {
        EMOTE_ONESHOT_WAVE,
        EMOTE_ONESHOT_CHEER,
        EMOTE_ONESHOT_LAUGH,
        EMOTE_ONESHOT_SALUTE,
        EMOTE_ONESHOT_APPLAUD,
        EMOTE_ONESHOT_POINT
    };

    bot->HandleEmoteCommand(emotes[urand(0, (sizeof(emotes) / sizeof(emotes[0])) - 1)]);
    botAI->GetAiObjectContext()->GetValue<time_t>("last emote", "emote")->Set(time(nullptr) + urand(5, 25));
    return true;
}

bool EmoteAction::isUseful()
{
    if (!botAI->AllowActivity())
        return false;

    time_t lastEmote = AI_VALUE2(time_t, "last emote", "emote");
    return time(nullptr) >= lastEmote;
}

GreetAction::GreetAction(PlayerbotAI* botAI) : Action(botAI, "greet") {}

bool GreetAction::Execute(Event /*event*/)
{
    ObjectGuid guid = AI_VALUE(ObjectGuid, "new player nearby");
    if (!guid || !guid.IsPlayer())
        return false;

    Player* player = ObjectAccessor::FindPlayer(guid);
    if (!player)
        return false;

    if (!bot->HasInArc(CAST_ANGLE_IN_FRONT, player, sPlayerbotAIConfig->sightDistance))
        bot->SetFacingToObject(player);

    ObjectGuid oldSel = bot->GetTarget();
    bot->SetTarget(guid);
    bot->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
    bot->SetTarget(oldSel);

    GuidSet& alreadySeenPlayers = botAI->GetAiObjectContext()->GetValue<GuidSet&>("already seen players")->Get();
    alreadySeenPlayers.insert(guid);

    GuidVector nearestPlayers = botAI->GetAiObjectContext()->GetValue<GuidVector>("nearest friendly players")->Get();
    for (ObjectGuid const nearbyGuid : nearestPlayers)
        alreadySeenPlayers.insert(nearbyGuid);

    return true;
}
