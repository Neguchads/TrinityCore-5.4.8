#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "DBUpdater.h"

enum ModExampleString
{
    HELLO_WORLD = 35410
};

class ModExamplePlayer : public PlayerScript
{
public:
    ModExamplePlayer() : PlayerScript("ModExamplePlayer") { }

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetBoolDefault("ModExample.Enable", false))
            ChatHandler(player->GetSession()).PSendSysMessage(HELLO_WORLD);
    }
};

class ModExampleDatabaseScript : public DatabaseScript
{
public:
    ModExampleDatabaseScript() : DatabaseScript("ModExampleDatabaseScript") {}

    bool OnDatabasesLoading() override
    {
        if (!sConfigMgr->GetBoolDefault("ModExample.Updates.EnableDatabases", true))
            return true;

        WorldDatabase.PExecute("INSERT IGNORE INTO updates_include (`path`, `state`) VALUES ('$/modules/mod-example/data/sql/world', 'RELEASED')");

        if (!DBUpdater<WorldDatabaseConnection>::Update(WorldDatabase))
            return false;

        return true;
    }
};

void AddExampleScripts()
{
    new ModExampleDatabaseScript();
    new ModExamplePlayer();
}
