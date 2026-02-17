/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "AuctionHouseBot.h"
#include "Chat.h"
#include "Config.h"
#include "ScriptMgr.h"

class AHBot_WorldScript : public WorldScript
{
public:
    AHBot_WorldScript() : WorldScript("AHBot_WorldScript"), _hasPerformedStartup(false), _updateAccumulator(0) { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        if (!auctionbot->IsModuleEnabled())
            return;

        auctionbot->InitializeConfiguration();

        if (_hasPerformedStartup)
            RepopulateCandidates();
    }

    void OnStartup() override
    {
        if (!auctionbot->IsModuleEnabled())
            return;

        auctionbot->InitializeConfiguration();
        RepopulateCandidates();
        _hasPerformedStartup = true;
    }

    void OnUpdate(uint32 diff) override
    {
        if (!auctionbot->IsModuleEnabled())
            return;

        _updateAccumulator += diff;
        while (_updateAccumulator >= MINUTE * IN_MILLISECONDS)
        {
            _updateAccumulator -= MINUTE * IN_MILLISECONDS;
            auctionbot->Update();
        }
    }

private:
    void RepopulateCandidates() const
    {
        LOG_INFO("server.loading", "AuctionHouseBot: (Re)populating item candidate lists ...");
        auctionbot->PopulateItemCandidatesAndProportions();

        if (sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedListingRules.UseDropRates.Enabled", false))
        {
            auctionbot->PopulateQuestRewardItemIDs();
            auctionbot->PopulateItemDropChances();
        }
    }

    bool _hasPerformedStartup;
    uint32 _updateAccumulator;
};

class AHBot_CommandScript : public CommandScript
{
public:
    AHBot_CommandScript() : CommandScript("AHBot_CommandScript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> AHBotCommandTable =
        {
            { "update", SEC_GAMEMASTER, true, &HandleAHBotUpdateCommand },
            { "reload", SEC_GAMEMASTER, true, &HandleAHBotReloadCommand },
            { "empty", SEC_GAMEMASTER, true, &HandleAHBotEmptyCommand },
            { "help", SEC_GAMEMASTER, true, &HandleAHBotHelpCommand }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "ahbot", SEC_GAMEMASTER, true, AHBotCommandTable }
        };

        return commandTable;
    }

    static bool HandleAHBotUpdateCommand(ChatHandler* handler, char const* /*args*/)
    {
        LOG_INFO("module", "AuctionHouseBot: Updating Auction House...");
        handler->PSendSysMessage("AuctionHouseBot: Updating Auction House...");
        auctionbot->Update();
        LOG_INFO("module", "AuctionHouseBot: Auction House Updated.");
        handler->PSendSysMessage("AuctionHouseBot: Auction House Updated.");
        return true;
    }

    static bool HandleAHBotReloadCommand(ChatHandler* handler, char const* /*args*/)
    {
        LOG_INFO("module", "AuctionHouseBot: Reloading Config...");
        handler->PSendSysMessage("AuctionHouseBot: Reloading Config...");

        sConfigMgr->LoadModulesConfigs();
        auctionbot->InitializeConfiguration();
        auctionbot->PopulateItemCandidatesAndProportions();

        if (sConfigMgr->GetOption<bool>("AuctionHouseBot.AdvancedListingRules.UseDropRates.Enabled", true))
        {
            auctionbot->PopulateQuestRewardItemIDs();
            auctionbot->PopulateItemDropChances();
        }

        LOG_INFO("module", "AuctionHouseBot: Config reloaded.");
        handler->PSendSysMessage("AuctionHouseBot: Config reloaded.");
        return true;
    }

    static bool HandleAHBotEmptyCommand(ChatHandler* handler, char const* /*args*/)
    {
        LOG_INFO("module", "AuctionHouseBot: Emptying Auction House...");
        handler->PSendSysMessage("AuctionHouseBot: Emptying Auction House...");
        auctionbot->EmptyAuctionHouses();
        auctionbot->CleanupExpiredAuctionItems(); // Must go after EmptyAuctionHouses()
        LOG_INFO("module", "AuctionHouseBot: Auction Houses Emptied.");
        handler->PSendSysMessage("AuctionHouseBot: Auction Houses Emptied.");
        return true;
    }

    static bool HandleAHBotHelpCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->PSendSysMessage("AuctionHouseBot commands:");
        handler->PSendSysMessage("  .ahbot reload - Reloads configuration");
        handler->PSendSysMessage("  .ahbot empty  - Removes all AuctionHouseBot auctions");
        handler->PSendSysMessage("  .ahbot update - Runs an update cycle");
        return true;
    }
};

void AddAHBotScripts()
{
    new AHBot_WorldScript();
    new AHBot_CommandScript();
}
