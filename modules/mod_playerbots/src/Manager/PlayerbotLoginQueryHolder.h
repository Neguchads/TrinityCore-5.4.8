#ifndef _PLAYERBOT_PLAYERBOTLOGINQUERYHOLDER_H
#define _PLAYERBOT_PLAYERBOTLOGINQUERYHOLDER_H

#include "CharacterHandler.h"

class PlayerbotHolder;

class PlayerbotLoginQueryHolder : public LoginQueryHolder
{
public:
    PlayerbotLoginQueryHolder(PlayerbotHolder* playerbotHolder, uint32 masterAccount, uint32 accountId, ObjectGuid guid)
        : LoginQueryHolder(accountId, guid),
          masterAccountId(masterAccount),
          playerbotHolder(playerbotHolder)
    {
    }

    uint32 GetMasterAccountId() const { return masterAccountId; }
    PlayerbotHolder* GetPlayerbotHolder() const { return playerbotHolder; }

private:
    uint32 masterAccountId = 0;
    PlayerbotHolder* playerbotHolder = nullptr;
};

#endif
