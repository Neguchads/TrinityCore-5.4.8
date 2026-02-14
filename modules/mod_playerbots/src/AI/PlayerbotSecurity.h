#ifndef _PLAYERBOT_SECURITY_H
#define _PLAYERBOT_SECURITY_H

#include <map>

#include "Common.h"

class Player;

enum class PlayerbotSecurityLevel : uint8
{
    PLAYERBOT_SECURITY_DENY_ALL = 0,
    PLAYERBOT_SECURITY_TALK = 1,
    PLAYERBOT_SECURITY_GUILD = 2,
    PLAYERBOT_SECURITY_INVITE = 3,
    PLAYERBOT_SECURITY_ALLOW_ALL = 4
};

enum class DenyReason : uint8
{
    PLAYERBOT_DENY_NONE,
    PLAYERBOT_DENY_NOT_YOURS,
    PLAYERBOT_DENY_OPPOSING,
    PLAYERBOT_DENY_DEAD,
    PLAYERBOT_DENY_FAR,
    PLAYERBOT_DENY_INVITE,
    PLAYERBOT_DENY_FULL_GROUP,
    PLAYERBOT_DENY_BG,
    PLAYERBOT_DENY_LFG
};

// Minimal security gate mainly for invites and control. Keep it simple: if the bot has a master, only the master
// can fully control it. If it has no master, default to permissive behaviour (legacy behaviour) and let higher-level
// logic decide.
class PlayerbotSecurity
{
public:
    PlayerbotSecurity() : bot(nullptr) {}
    explicit PlayerbotSecurity(Player* const bot);

    PlayerbotSecurityLevel LevelFor(Player* from, DenyReason* reason = nullptr, bool ignoreGroup = false);
    bool CheckLevelFor(PlayerbotSecurityLevel level, bool silent, Player* from, bool ignoreGroup = false);

private:
    Player* bot;
    uint32 account = 0;
    std::map<uint64, std::map<std::string, time_t>> whispers;
};

#endif
