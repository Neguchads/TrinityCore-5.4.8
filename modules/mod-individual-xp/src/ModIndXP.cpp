#include "ScriptMgr.h"
#include "WorldSession.h"
#include "Player.h"
#include "SharedDefines.h"
#include "Config.h"
#include "Chat.h"
#include "Object.h"
#include "DatabaseEnv.h"
#include "DBUpdater.h"
#include "Log.h"
#include <unordered_map>

enum IndividualXPString
{
    IXP_HELLO = 100040,
    IXP_MODULE_DISABLED,
    IXP_RATES_DISABLED,
    IXP_COMMAND_VIEW,
    IXP_MAX_RATE,
    IXP_MIN_RATE,
    IXP_COMMAND_SET,
    IXP_COMMAND_DISABLED,
    IXP_COMMAND_ENABLED,
    IXP_COMMAND_DEFAULT
};

static bool moduleEnabled = true;
static uint32 maxRate = 10;
static uint32 defaultRate = 1;
static std::unordered_map<uint32, uint32> sRatesByGuid;

static void LoadConfig()
{
    moduleEnabled = sConfigMgr->GetBoolDefault("IndividualXP.Enabled", true);
    maxRate = sConfigMgr->GetIntDefault("IndividualXP.MaxRate", 10);
    defaultRate = sConfigMgr->GetIntDefault("IndividualXP.DefaultRate", 1);

    TC_LOG_INFO("server.loading", "mod-individual-xp: Enabled=%u MaxRate=%u DefaultRate=%u Announce=%u",
        uint32(moduleEnabled),
        maxRate,
        defaultRate,
        sConfigMgr->GetBoolDefault("IndividualXP.Announce", true) ? 1u : 0u);
}

class IndividualXPConfig : public WorldScript
{
public:
    IndividualXPConfig() : WorldScript("IndividualXPConfig") {}

    void OnBeforeConfigLoad(bool reload) override
    {
        LoadConfig();
    }
};

static uint32 GetPlayerRate(Player* player)
{
    uint32 guid = player->GetGUID().GetCounter();
    auto it = sRatesByGuid.find(guid);
    if (it != sRatesByGuid.end())
        return it->second;

    return defaultRate;
}

static void SetPlayerRate(Player* player, uint32 rate)
{
    uint32 guid = player->GetGUID().GetCounter();
    sRatesByGuid[guid] = rate;
}

class IndividualXPDatabaseScript : public DatabaseScript
{
public:
    IndividualXPDatabaseScript() : DatabaseScript("IndividualXPDatabaseScript") {}

    bool OnDatabasesLoading() override
    {
        if (!sConfigMgr->GetBoolDefault("IndividualXP.Updates.EnableDatabases", true))
            return true;

        CharacterDatabase.PExecute("INSERT IGNORE INTO updates_include (`path`, `state`) VALUES ('$/modules/mod-individual-xp/data/sql/db-characters', 'RELEASED')");
        WorldDatabase.PExecute("INSERT IGNORE INTO updates_include (`path`, `state`) VALUES ('$/modules/mod-individual-xp/data/sql/db-world', 'RELEASED')");

        if (!DBUpdater<CharacterDatabaseConnection>::Update(CharacterDatabase))
            return false;
        if (!DBUpdater<WorldDatabaseConnection>::Update(WorldDatabase))
            return false;

        return true;
    }
};

// Add IndividualXP Announcer
class IndividualXP_Announce : public PlayerScript
{
  public:
    IndividualXP_Announce() : PlayerScript("IndividualXP_Announcer") {}

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetBoolDefault("IndividualXP.Announce", true))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(IXP_HELLO);
        }
    }
};

class IndividualXP : public PlayerScript
{
  public:
    IndividualXP() : PlayerScript("IndividualXP") {}

    void OnLogin(Player* player) override
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT `Rate` FROM `individual_xp` WHERE `CharacterGUID` = %u", player->GetGUID().GetCounter());
        if (!result)
        {
            SetPlayerRate(player, defaultRate);
        }
        else
        {
            Field* fields = result->Fetch();
            uint32 rate = fields[0].GetUInt32();
            if (rate == 0)
                rate = defaultRate;
            SetPlayerRate(player, rate);
        }
    }

    void OnLogout(Player* player) override
    {
        CharacterDatabase.PExecute("REPLACE INTO `individual_xp` (`CharacterGUID`, `Rate`) VALUES ('%u', '%u');",
            player->GetGUID().GetCounter(), GetPlayerRate(player));
        sRatesByGuid.erase(player->GetGUID().GetCounter());
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* /*victim*/) override
    {
        if (moduleEnabled && !player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN))
            amount *= GetPlayerRate(player);
    }
};

class IndividualXPCommandScript : public CommandScript
{
  public:
    IndividualXPCommandScript() : CommandScript("IndividualXPCommand") {}

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> IndividualXPCommandTable =
        {
            { "enable",  SEC_PLAYER,  false, &HandleEnableCommand },
            { "disable", SEC_PLAYER,  false, &HandleDisableCommand },
            { "view",    SEC_PLAYER,  false, &HandleViewCommand },
            { "set",     SEC_PLAYER,  false, &HandleSetCommand },
            { "default", SEC_PLAYER,  false, &HandleDefaultCommand },
        };
        static std::vector<ChatCommand> IndividualXPBaseTable =
        {
            { "xp", SEC_PLAYER, false, IndividualXPCommandTable },
        };
        return IndividualXPBaseTable;
    }

    static bool HandleViewCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (!moduleEnabled)
        {
            handler->PSendSysMessage(IXP_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;

        if (me->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN))
        {
            handler->PSendSysMessage(IXP_RATES_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }
        else
        {
            handler->PSendSysMessage(IXP_COMMAND_VIEW, GetPlayerRate(me));
        }
        return true;
    }

    // Set Command
    static bool HandleSetCommand(ChatHandler* handler, char const* args)
    {
        if (!moduleEnabled)
        {
            handler->PSendSysMessage(IXP_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!*args)
            return false;

        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;

        uint32 rate = (uint32)atol(args);
        if (rate > maxRate)
        {
            handler->PSendSysMessage(IXP_MAX_RATE, maxRate);
            handler->SetSentErrorMessage(true);
            return false;
        }
        else if (rate == 0)
        {
            handler->PSendSysMessage(IXP_MIN_RATE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        SetPlayerRate(me, rate);
        handler->PSendSysMessage(IXP_COMMAND_SET, rate);
        return true;
    }

    // Disable Command
    static bool HandleDisableCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (!moduleEnabled)
        {
            handler->PSendSysMessage(IXP_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;

        // Turn Disabled On But Don't Change Value...
        me->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        handler->PSendSysMessage(IXP_COMMAND_DISABLED);
        return true;
    }

    // Enable Command
    static bool HandleEnableCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (!moduleEnabled)
        {
            handler->PSendSysMessage(IXP_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;

        me->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        handler->PSendSysMessage(IXP_COMMAND_ENABLED);
        return true;
    }

    // Default Command
    static bool HandleDefaultCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (!moduleEnabled)
        {
            handler->PSendSysMessage(IXP_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* me = handler->GetSession()->GetPlayer();
        if (!me)
            return false;

        SetPlayerRate(me, defaultRate);
        handler->PSendSysMessage(IXP_COMMAND_DEFAULT, defaultRate);
        return true;
    }
};

// Add all scripts in one
void AddModIndXPScripts()
{
    new IndividualXPConfig();
    new IndividualXPDatabaseScript();
    new IndividualXP_Announce();
    new IndividualXP();
    new IndividualXPCommandScript();
}
