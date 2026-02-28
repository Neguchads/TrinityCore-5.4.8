#ifndef _PLAYERBOT_PLAYERBOTLOGINMGR_H
#define _PLAYERBOT_PLAYERBOTLOGINMGR_H

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "AsyncCallbackProcessor.h"
#include "Common.h"
#include "DatabaseEnvFwd.h"
#include "Player.h"
#include "Position.h"
#include "SharedDefines.h"

class Player;
class PlayerbotLoginQueryHolder;

typedef std::map<uint32, Player*> RealPlayers;

typedef std::vector<std::string> LoginCriteriaConfig;

struct LoginSpace
{
    int32 currentSpace = 0;
    int32 totalSpace = 0;
    int32 classRaceBucket[MAX_CLASSES][MAX_RACES] = {};
    int32 levelBucket[DEFAULT_MAX_LEVEL + 1] = {};
};

enum class HolderState : uint8
{
    HOLDER_EMPTY = 0,
    HOLDER_SENT = 1,
    HOLDER_RECEIVED = 2
};

enum class LoginState : uint8
{
    BOT_OFFLINE = 0,
    BOT_ON_LOGINQUEUE = 1,
    BOT_ONLINE = 2,
    BOT_ON_LOGOUTQUEUE = 3
};

enum class FillStep : uint8
{
    NOW = 0,
    NEXT_STEP = 1,
    ALL_POTENTIAL = 2
};

enum class LoginCriterionFailType : uint8
{
    UNKNOWN = 0,
    MAX_BOTS = 1,
    SPARE_ROOM,
    ONLINE,
    RANDOM_TIMED_LOGOUT,
    RANDOM_TIMED_OFFLINE,
    CLASSRACE,
    LEVEL,
    RANGE,
    MAP,
    GROUP,
    GUILD,
    BG,
    ARENA,
    INSTANCE,
    LOGIN_OK
};

class PlayerLoginInfo;
typedef std::map<uint32, PlayerLoginInfo> BotPool;
typedef std::vector<PlayerLoginInfo> RealPlayerInfos;
typedef std::vector<PlayerLoginInfo*> BotInfos;
typedef std::vector<std::pair<LoginCriterionFailType, std::function<bool(PlayerLoginInfo const&, LoginSpace const&, RealPlayerInfos const&)>>> LoginCriteria;

class PlayerLoginInfo
{
public:
    PlayerLoginInfo() = default;
    PlayerLoginInfo(uint32 account, uint32 guid, uint8 race, uint8 cls, uint32 level, bool isNew, WorldLocation const& position, uint32 guildId);
    explicit PlayerLoginInfo(Player* player);

    ObjectGuid GetGuid() const { return ObjectGuid::Create<HighGuid::Player>(guid); }
    uint32 GetId() const { return guid; }
    uint8 GetRace() const { return race; }
    uint8 GetClass() const { return cls; }
    uint32 GetLevel() const { return level; }
    Player* GetPlayer() const;
    bool IsNearPlayer(RealPlayerInfos const& space) const;
    bool IsOnPlayerMap(RealPlayerInfos const& space) const;
    bool IsInPlayerGroup(RealPlayerInfos const& space) const;
    bool IsInPlayerGuild(RealPlayerInfos const& space) const;
    bool IsInBG() const;
    bool IsInArena() const;
    bool IsInInstance() const;
    bool IsOnline() const { return loginState == LoginState::BOT_ONLINE || loginState == LoginState::BOT_ON_LOGOUTQUEUE; }
    LoginState GetLoginState() const { return loginState; }

    bool SendHolder();
    void HandlePlayerBotLoginCallback(std::shared_ptr<PlayerbotLoginQueryHolder> const& expectedHolder);
    void ResetLoginState();
    void FillLoginSpace(LoginSpace& space, FillStep step = FillStep::NOW) const;
    void EmptyLoginSpace(LoginSpace& space, FillStep step = FillStep::NOW) const;
    void SetQueue(bool isWanted, LoginSpace& space);
    bool IsQueued() const;
    LoginCriterionFailType MatchNoCriteria(LoginSpace const& space, LoginCriteria const& criteria, RealPlayerInfos const& realPlayers) const;
    void Update(Player* player);
    void MarkOffline();
    bool LoginBot();
    bool LogoutBot();

private:
    uint32 account = 0;
    uint32 guid = 0;
    uint8 race = 0;
    uint8 cls = 0;
    uint32 level = 1;
    bool isNew = false;
    WorldLocation position;
    uint32 groupId = 0;
    uint32 guildId = 0;
    std::shared_ptr<PlayerbotLoginQueryHolder> holder;
    HolderState holderState = HolderState::HOLDER_EMPTY;
    LoginState loginState = LoginState::BOT_OFFLINE;
    uint32 lastLoginAttempt = 0;
    uint32 lastLogoutAttempt = 0;
};

class PlayerBotLoginMgr
{
public:
    static PlayerBotLoginMgr* instance()
    {
        static PlayerBotLoginMgr instance;
        return &instance;
    }

    void Update(RealPlayers& realPlayers);
    void ToggleDebug() { debug = !debug; }

private:
    static BotPool LoadBotsFromDb();
    void UpdateOnlineBots();
    static BotInfos FillLoginLogoutQueue(BotPool* pool, RealPlayers const& realPlayers);
    void LoginLogoutBots(BotInfos const& queue);

    static RealPlayerInfos GetPlayerInfos(RealPlayers const& realPlayers);
    static uint32 GetLoginCriteriaSize();
    static std::vector<std::string> GetVariableLoginCriteria(uint8 attempt);
    static LoginCriteria GetLoginCriteria(uint8 attempt);
    static bool CriteriaStillValid(LoginCriterionFailType oldFailType, LoginCriteria const& criteria);

    static uint32 GetMaxOnlineBotCount();
    static uint32 GetMaxLevel();
    static uint32 GetClassRaceBucketSize(uint8 cls, uint8 race);
    static uint32 GetLevelBucketSize(uint32 level);
    static void FillLoginSpace(BotPool* pool, LoginSpace& space, FillStep step);

    void SendHolders(BotInfos const& queue);
    void SendHolders(BotPool* pool);

private:
    bool debug = false;
    BotInfos onlineBots;
    BotPool botPool;
    QueryCallbackProcessor _queryProcessor;
};

#define sPlayerBotLoginMgr PlayerBotLoginMgr::instance()

#endif
