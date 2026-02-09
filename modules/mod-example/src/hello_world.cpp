#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

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

void AddExampleScripts()
{
    new ModExamplePlayer();
}
