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

#include "RpgSubActions.h"

#include <cmath>

#include "ChooseRpgTargetAction.h"
#include "ChatActions.h"
#include "ChatHelper.h"
#include "Formations.h"
#include "GossipDef.h"
#include "LastMovementValue.h"
#include "MovementActions.h"
#include "ItemVisitors.h"
#include "Playerbots.h"
#include "PossibleRpgTargetsValue.h"
#include "SocialMgr.h"
#include "TradeData.h"

namespace
{
void BuildGossipHelloPacket(WorldPacket& packet, ObjectGuid const& guid)
{
    packet.WriteBit(guid[2]);
    packet.WriteBit(guid[4]);
    packet.WriteBit(guid[0]);
    packet.WriteBit(guid[3]);
    packet.WriteBit(guid[6]);
    packet.WriteBit(guid[7]);
    packet.WriteBit(guid[5]);
    packet.WriteBit(guid[1]);

    packet.WriteByteSeq(guid[4]);
    packet.WriteByteSeq(guid[7]);
    packet.WriteByteSeq(guid[1]);
    packet.WriteByteSeq(guid[0]);
    packet.WriteByteSeq(guid[5]);
    packet.WriteByteSeq(guid[3]);
    packet.WriteByteSeq(guid[6]);
    packet.WriteByteSeq(guid[2]);
}
} // namespace

void RpgHelper::OnExecute(std::string nextAction)
{
    if (botAI->HasRealPlayerMaster() && nextAction == "rpg")
        nextAction = "rpg cancel";

    SET_AI_VALUE(std::string, "next rpg action", nextAction);
}

void RpgHelper::BeforeExecute()
{
    OnExecute();

    bot->SetTarget(guidP());
    setFacingTo(guidP());
}

void RpgHelper::AfterExecute(bool doDelay, bool waitForGroup)
{
    OnExecute();

    bot->SetTarget(guidP());
    setFacingTo(guidP());

    if (doDelay)
        setDelay(waitForGroup);

    setFacing(guidP());
}

GuidPosition RpgHelper::guidP() { return AI_VALUE(GuidPosition, "rpg target"); }

ObjectGuid RpgHelper::guid() { return static_cast<ObjectGuid>(guidP()); }

bool RpgHelper::InRange()
{
    return guidP() ? (guidP().sqDistance2d(bot) < INTERACTION_DISTANCE * INTERACTION_DISTANCE) : false;
}

void RpgHelper::setFacingTo(GuidPosition guidPosition)
{
    bot->SetFacingTo(guidPosition.getAngleTo(bot) + static_cast<float>(M_PI));
}

void RpgHelper::setFacing(GuidPosition guidPosition)
{
    if (!guidPosition.IsUnit())
        return;

    if (guidPosition.IsPlayer())
        return;
}

void RpgHelper::setDelay(bool waitForGroup)
{
    if (!botAI->HasRealPlayerMaster() || (waitForGroup && botAI->GetGroupLeader() == bot && bot->GetGroup()))
        botAI->SetNextCheckDelay(sPlayerbotAIConfig->rpgDelay);
    else
        botAI->SetNextCheckDelay(sPlayerbotAIConfig->rpgDelay / 5);
}

bool RpgSubAction::isPossible()
{
    if (!rpg->guidP() || !rpg->guidP().GetWorldObject())
        return false;

    std::string actionName = ActionName();
    if (actionName != "none" && !botAI->GetAiObjectContext()->GetAction(actionName))
        return false;

    return true;
}

bool RpgSubAction::isUseful() { return rpg->InRange(); }

bool RpgSubAction::Execute(Event event)
{
    bool doAction = botAI->DoSpecificAction(ActionName(), ActionEvent(event), true);
    rpg->AfterExecute(doAction, true);
    return doAction;
}

std::string const RpgSubAction::ActionName() { return "none"; }

Event RpgSubAction::ActionEvent(Event event) { return event; }

bool RpgStayAction::isUseful() { return rpg->InRange() && !botAI->HasRealPlayerMaster(); }

bool RpgStayAction::Execute(Event /*event*/)
{
    bot->PlayerTalkClass->SendCloseGossip();

    rpg->AfterExecute();
    return true;
}

bool RpgWorkAction::isUseful() { return rpg->InRange() && !botAI->HasRealPlayerMaster(); }

bool RpgWorkAction::Execute(Event /*event*/)
{
    bot->HandleEmoteCommand(EMOTE_STATE_USE_STANDING);
    rpg->AfterExecute();
    return true;
}

bool RpgEmoteAction::isUseful() { return rpg->InRange() && !botAI->HasRealPlayerMaster(); }

bool RpgEmoteAction::Execute(Event /*event*/)
{
    uint32 type = EMOTE_ONESHOT_TALK;

    ObjectGuid guid = rpg->guid();
    WorldPacket p1;
    BuildGossipHelloPacket(p1, guid);
    bot->GetSession()->HandleGossipHelloOpcode(p1);

    bot->HandleEmoteCommand(type);

    rpg->AfterExecute();

    return true;
}

bool RpgCancelAction::Execute(Event /*event*/)
{
    RESET_AI_VALUE(GuidPosition, "rpg target");
    rpg->OnExecute("");
    return true;
}

bool RpgTaxiAction::isUseful() { return rpg->InRange() && !botAI->HasRealPlayerMaster(); }

bool RpgTaxiAction::Execute(Event /*event*/)
{
    GuidPosition guidP = rpg->guidP();

    WorldPacket emptyPacket;
    bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);

    uint32 node = sObjectMgr->GetNearestTaxiNode(guidP.getX(), guidP.getY(), guidP.getZ(), guidP.getMapId(), bot->GetTeamId());

    std::vector<uint32> nodes;
    for (uint32 i = 0; i < sTaxiPathStore.GetNumRows(); ++i)
    {
        TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(i);
        if (entry && entry->from == node && (bot->m_taxi.IsTaximaskNodeKnown(entry->to) || bot->isTaxiCheater()))
            nodes.push_back(i);
    }

    if (nodes.empty())
    {
        TC_LOG_ERROR("playerbots", "Bot {} - No flight paths available", bot->GetName());
        return false;
    }

    uint32 path = nodes[urand(0, nodes.size() - 1)];
    uint32 money = bot->GetMoney();
    bot->SetMoney(money + 100000);

    TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(path);
    if (!entry)
        return false;

    TaxiNodesEntry const* nodeFrom = sTaxiNodesStore.LookupEntry(entry->from);
    TaxiNodesEntry const* nodeTo = sTaxiNodesStore.LookupEntry(entry->to);

    Creature* flightMaster = bot->GetNPCIfCanInteractWith(guidP, UNIT_NPC_FLAG_FLIGHTMASTER);
    if (!flightMaster)
    {
        TC_LOG_ERROR("playerbots", "Bot {} cannot talk to flightmaster ({} location available)", bot->GetName(), nodes.size());
        return false;
    }

    if (!bot->ActivateTaxiPathTo({ entry->from, entry->to }, flightMaster, 0))
    {
        TC_LOG_ERROR("playerbots", "Bot {} cannot fly {} ({} location available)", bot->GetName(), path, nodes.size());
        return false;
    }

    TC_LOG_INFO("playerbots", "Bot {} <{}> is flying from {} to {} ({} location available)",
        bot->GetGUID().ToString().c_str(), bot->GetName(), nodeFrom->name[0], nodeTo->name[0], nodes.size());

    bot->SetMoney(money);
    rpg->AfterExecute();

    return true;
}

bool RpgDiscoverAction::Execute(Event /*event*/)
{
    GuidPosition guidP = rpg->guidP();

    uint32 node = sObjectMgr->GetNearestTaxiNode(guidP.getX(), guidP.getY(), guidP.getZ(), guidP.getMapId(), bot->GetTeamId());
    if (!node)
        return false;

    Creature* flightMaster = bot->GetNPCIfCanInteractWith(guidP, UNIT_NPC_FLAG_FLIGHTMASTER);
    if (!flightMaster)
        return false;

    return bot->GetSession()->SendLearnNewTaxiNode(flightMaster);
}

std::string const RpgStartQuestAction::ActionName() { return "accept all quests"; }

Event RpgStartQuestAction::ActionEvent(Event /*event*/)
{
    WorldPacket p(CMSG_QUESTGIVER_ACCEPT_QUEST);
    p << rpg->guid();
    p.rpos(0);
    return Event("rpg action", p);
}

std::string const RpgEndQuestAction::ActionName() { return "talk to quest giver"; }

Event RpgEndQuestAction::ActionEvent(Event /*event*/)
{
    WorldPacket p(CMSG_QUESTGIVER_COMPLETE_QUEST);
    p << rpg->guid();
    p.rpos(0);
    return Event("rpg action", p);
}

std::string const RpgBuyAction::ActionName() { return "buy"; }

Event RpgBuyAction::ActionEvent(Event /*event*/) { return Event("rpg action", "vendor"); }

std::string const RpgSellAction::ActionName() { return "sell"; }

Event RpgSellAction::ActionEvent(Event /*event*/) { return Event("rpg action", "vendor"); }

std::string const RpgRepairAction::ActionName() { return "repair"; }

std::string const RpgTrainAction::ActionName() { return "trainer"; }

bool RpgHealAction::Execute(Event /*event*/)
{
    bool retVal = false;

    switch (bot->GetClass())
    {
        case CLASS_PRIEST:
            retVal = botAI->DoSpecificAction("lesser heal on party", Event(), true);
            break;
        case CLASS_DRUID:
            retVal = botAI->DoSpecificAction("healing touch on party", Event(), true);
            break;
        case CLASS_PALADIN:
            retVal = botAI->DoSpecificAction("holy light on party", Event(), true);
            break;
        case CLASS_SHAMAN:
            retVal = botAI->DoSpecificAction("healing wave on party", Event(), true);
            break;
    }

    return retVal;
}

std::string const RpgHomeBindAction::ActionName() { return "home"; }

std::string const RpgQueueBgAction::ActionName()
{
    SET_AI_VALUE(uint32, "bg type", (uint32)AI_VALUE(BattlegroundTypeId, "rpg bg type"));
    return "free bg join";
}

std::string const RpgBuyPetitionAction::ActionName() { return "buy petition"; }

std::string const RpgUseAction::ActionName() { return "use"; }

Event RpgUseAction::ActionEvent(Event /*event*/)
{
    return Event("rpg action", ChatHelper::FormatWorldobject(rpg->guidP().GetWorldObject()));
}

std::string const RpgSpellAction::ActionName() { return "cast random spell"; }

Event RpgSpellAction::ActionEvent(Event /*event*/)
{
    return Event("rpg action", ChatHelper::FormatWorldobject(rpg->guidP().GetWorldObject()));
}

std::string const RpgCraftAction::ActionName() { return "craft random item"; }

Event RpgCraftAction::ActionEvent(Event /*event*/)
{
    return Event("rpg action", ChatHelper::FormatWorldobject(rpg->guidP().GetWorldObject()));
}

std::vector<Item*> RpgTradeUsefulAction::CanGiveItems(GuidPosition guidPosition)
{
    Player* player = guidPosition.GetPlayer();

    std::vector<Item*> giveItems;

    if (botAI->HasActivePlayerMaster() || !GET_PLAYERBOT_AI(player))
        return giveItems;

    std::vector<ItemUsage> myUsages = { ITEM_USAGE_NONE, ITEM_USAGE_VENDOR, ITEM_USAGE_AH, ITEM_USAGE_DISENCHANT };

    for (auto& myUsage : myUsages)
    {
        std::vector<Item*> myItems =
            AI_VALUE2(std::vector<Item*>, "inventory items", "usage " + std::to_string(static_cast<uint32>(myUsage)));
        std::reverse(myItems.begin(), myItems.end());

        for (auto& item : myItems)
        {
            if (!item->CanBeTraded())
                continue;

            if (bot->GetTradeData() && bot->GetTradeData()->HasItem(item->GetGUID()))
                continue;

            ItemUsage otherUsage = PAI_VALUE2(ItemUsage, "item usage", item->GetEntry());

            bool isKnownUsage = false;
            for (ItemUsage usage : myUsages)
            {
                if (usage == otherUsage)
                {
                    isKnownUsage = true;
                    break;
                }
            }
            if (!isKnownUsage)
                giveItems.push_back(item);
        }
    }

    return giveItems;
}

bool RpgTradeUsefulAction::Execute(Event /*event*/)
{
    GuidPosition guidP = AI_VALUE(GuidPosition, "rpg target");

    Player* player = guidP.GetPlayer();

    if (!player)
        return false;

    std::vector<Item*> items = CanGiveItems(guidP);

    if (items.empty())
        return false;

    Item* item = items.front();

    std::ostringstream param;
    param << ChatHelper::FormatWorldobject(player) << " " << ChatHelper::FormatItem(item->GetTemplate());

    bool hasTraded = botAI->DoSpecificAction("trade", Event("rpg action", param.str().c_str()), true);

    if (hasTraded || bot->GetTradeData())
    {
        if (bot->GetTradeData() && bot->GetTradeData()->HasItem(item->GetGUID()))
        {
            if (bot->GetGroup() && bot->GetGroup()->IsMember(guidP) && botAI->HasRealPlayerMaster())
                botAI->TellMasterNoFacing(
                    "You can use this " + ChatHelper::FormatItem(item->GetTemplate()) + " better than me, " +
                    guidP.GetPlayer()->GetName() + ".");
            else
                bot->Say("You can use this " + ChatHelper::FormatItem(item->GetTemplate()) + " better than me, " +
                    player->GetName() + ".",
                    (bot->GetTeamId() == TEAM_ALLIANCE ? LANG_COMMON : LANG_ORCISH));

            if (!urand(0, 4) || items.size() < 2)
            {
                WorldPacket p;
                uint32 status = TRADE_STATUS_ACCEPTED;
                p << status;
                bot->GetSession()->HandleAcceptTradeOpcode(p);
            }
        }
        else
            bot->Say("Start trade with" + ChatHelper::FormatWorldobject(player),
                (bot->GetTeamId() == TEAM_ALLIANCE ? LANG_COMMON : LANG_ORCISH));

        botAI->SetNextCheckDelay(sPlayerbotAIConfig->rpgDelay);
        return true;
    }

    return false;
}

bool RpgDuelAction::isUseful()
{
    if (sPlayerbotAIConfig->IsInPvpProhibitedZone(bot->GetZoneId()))
        return false;

    AreaTableEntry const* casterAreaEntry = sAreaTableStore.LookupEntry(bot->GetAreaId());
    if (casterAreaEntry && !(casterAreaEntry->Flags & AREA_FLAG_ALLOW_DUELS))
        return false;

    return true;
}

bool RpgDuelAction::Execute(Event /*event*/)
{
    GuidPosition guidP = AI_VALUE(GuidPosition, "rpg target");

    Player* player = guidP.GetPlayer();

    if (!player)
        return false;

    return botAI->DoSpecificAction("cast custom spell", Event("rpg action", ChatHelper::FormatWorldobject(player) + " 7266"), true);
}

bool RpgMountAnimAction::isUseful()
{
    return AI_VALUE2(bool, "mounted", "self target") && !AI_VALUE2(bool, "moving", "self target");
}

bool RpgMountAnimAction::Execute(Event /*event*/)
{
    WorldPacket p;
    bot->GetSession()->HandleMountSpecialAnimOpcode(p);
    return true;
}
