#include "PlayerbotLoginMgr.h"

#include <algorithm>
#include <chrono>
#include <cmath>

#include "CharacterCache.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "Group.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotLoginQueryHolder.h"
#include "RandomPlayerbotMgr.h"
#include "Timer.h"
#include "World.h"

namespace
{
    uint32 const loginAttemptDelayMs = 1500;
    uint32 const logoutAttemptDelayMs = 1500;
}

PlayerLoginInfo::PlayerLoginInfo(
    uint32 account,
    uint32 guid,
    uint8 race,
    uint8 cls,
    uint32 level,
    bool isNew,
    WorldLocation const& position,
    uint32 guildId)
    : account(account),
      guid(guid),
      race(race),
      cls(cls),
      level(level),
      isNew(isNew),
      position(position),
      guildId(guildId)
{
}

PlayerLoginInfo::PlayerLoginInfo(Player* player)
{
    if (!player)
        return;

    account = player->GetSession() ? player->GetSession()->GetAccountId() : 0;
    guid = player->GetGUID().GetCounter();
    race = static_cast<uint8>(player->GetRace());
    cls = static_cast<uint8>(player->GetClass());
    level = player->GetLevel();
    isNew = player->GetTotalPlayedTime() == 0;
    position = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
    groupId = player->GetGroup() ? player->GetGroup()->GetGUID().GetCounter() : 0;
    guildId = player->GetGuildId();
    loginState = LoginState::BOT_ONLINE;
}

Player* PlayerLoginInfo::GetPlayer() const
{
    return ObjectAccessor::FindConnectedPlayer(GetGuid());
}

bool PlayerLoginInfo::IsNearPlayer(RealPlayerInfos const& space) const
{
    if (space.empty())
        return false;

    float range = static_cast<float>(sPlayerbotAIConfig->loginBotsNearPlayerRange);
    float sqRange = range * range;

    for (PlayerLoginInfo const& playerInfo : space)
    {
        if (playerInfo.position.GetMapId() != position.GetMapId())
            continue;

        float dx = playerInfo.position.GetPositionX() - position.GetPositionX();
        float dy = playerInfo.position.GetPositionY() - position.GetPositionY();
        if ((dx * dx + dy * dy) <= sqRange)
            return true;
    }

    return false;
}

bool PlayerLoginInfo::IsOnPlayerMap(RealPlayerInfos const& space) const
{
    if (space.empty())
        return false;

    for (PlayerLoginInfo const& playerInfo : space)
        if (playerInfo.position.GetMapId() == position.GetMapId())
            return true;

    return false;
}

bool PlayerLoginInfo::IsInPlayerGroup(RealPlayerInfos const& space) const
{
    if (space.empty() || !groupId)
        return false;

    for (PlayerLoginInfo const& playerInfo : space)
        if (playerInfo.groupId == groupId)
            return true;

    return false;
}

bool PlayerLoginInfo::IsInPlayerGuild(RealPlayerInfos const& space) const
{
    if (space.empty() || !guildId)
        return false;

    for (PlayerLoginInfo const& playerInfo : space)
        if (playerInfo.guildId == guildId)
            return true;

    return false;
}

bool PlayerLoginInfo::IsInBG() const
{
    Player* player = GetPlayer();
    if (player)
        return player->InBattleground();

    MapEntry const* mapEntry = sMapStore.LookupEntry(position.GetMapId());
    return mapEntry && mapEntry->IsBattleground();
}

bool PlayerLoginInfo::IsInArena() const
{
    Player* player = GetPlayer();
    if (player)
        return player->InArena();

    MapEntry const* mapEntry = sMapStore.LookupEntry(position.GetMapId());
    return mapEntry && mapEntry->IsBattleArena();
}

bool PlayerLoginInfo::IsInInstance() const
{
    Player* player = GetPlayer();
    if (player && player->GetMap())
        return player->GetMap()->Instanceable() && !player->InBattleground() && !player->InArena();

    MapEntry const* mapEntry = sMapStore.LookupEntry(position.GetMapId());
    return mapEntry && mapEntry->Instanceable() && !mapEntry->IsBattleground() && !mapEntry->IsBattleArena();
}

bool PlayerLoginInfo::SendHolder()
{
    if (holderState == HolderState::HOLDER_SENT)
        return true;

    if (holderState == HolderState::HOLDER_RECEIVED)
        return false;

    if (loginState == LoginState::BOT_ONLINE ||
        loginState == LoginState::BOT_ON_LOGOUTQUEUE ||
        loginState == LoginState::BOT_OFFLINE)
        return false;

    sRandomPlayerbotMgr->GetValue(guid, "logout");

    holderState = HolderState::HOLDER_SENT;

    holder.reset();
    holder = std::make_shared<PlayerbotLoginQueryHolder>(sRandomPlayerbotMgr, 0, account, GetGuid());
    if (!holder->Initialize())
    {
        holder.reset();
        holderState = HolderState::HOLDER_EMPTY;
        return false;
    }

    auto callbackHolder = holder;
    sWorld->AddQueryHolderCallback(CharacterDatabase.DelayQueryHolder(holder)).AfterComplete(
        [this, callbackHolder](SQLQueryHolderBase const&)
        {
            HandlePlayerBotLoginCallback(callbackHolder);
        });

    return true;
}

void PlayerLoginInfo::HandlePlayerBotLoginCallback(std::shared_ptr<PlayerbotLoginQueryHolder> const& expectedHolder)
{
    if (!expectedHolder || holder != expectedHolder)
        return;

    holderState = HolderState::HOLDER_RECEIVED;
}

void PlayerLoginInfo::ResetLoginState()
{
    if (loginState == LoginState::BOT_ON_LOGINQUEUE)
        loginState = LoginState::BOT_OFFLINE;
    else if (loginState == LoginState::BOT_ON_LOGOUTQUEUE)
        loginState = LoginState::BOT_ONLINE;
}

void PlayerLoginInfo::FillLoginSpace(LoginSpace& space, FillStep step) const
{
    if (step == FillStep::NOW && (loginState == LoginState::BOT_OFFLINE || loginState == LoginState::BOT_ON_LOGINQUEUE))
        return;

    if (step == FillStep::NEXT_STEP && (loginState == LoginState::BOT_OFFLINE || loginState == LoginState::BOT_ON_LOGOUTQUEUE))
        return;

    space.totalSpace--;
    if (cls < MAX_CLASSES && race < MAX_RACES)
        space.classRaceBucket[cls][race]--;
    if (GetLevel() <= DEFAULT_MAX_LEVEL)
        space.levelBucket[GetLevel()]--;
}

void PlayerLoginInfo::EmptyLoginSpace(LoginSpace& space, FillStep step) const
{
    if (step == FillStep::NOW && (loginState == LoginState::BOT_OFFLINE || loginState == LoginState::BOT_ON_LOGINQUEUE))
        return;

    if (step == FillStep::NEXT_STEP && (loginState == LoginState::BOT_OFFLINE || loginState == LoginState::BOT_ON_LOGOUTQUEUE))
        return;

    space.totalSpace++;
    if (cls < MAX_CLASSES && race < MAX_RACES)
        space.classRaceBucket[cls][race]++;
    if (GetLevel() <= DEFAULT_MAX_LEVEL)
        space.levelBucket[GetLevel()]++;
}

void PlayerLoginInfo::SetQueue(bool isWanted, LoginSpace& space)
{
    if (isWanted)
    {
        if (loginState == LoginState::BOT_OFFLINE)
        {
            loginState = LoginState::BOT_ON_LOGINQUEUE;
            FillLoginSpace(space, FillStep::NEXT_STEP);
        }
        else if (loginState == LoginState::BOT_ON_LOGOUTQUEUE)
        {
            loginState = LoginState::BOT_ONLINE;
            FillLoginSpace(space, FillStep::NEXT_STEP);
        }
    }
    else
    {
        if (loginState == LoginState::BOT_ONLINE)
        {
            EmptyLoginSpace(space, FillStep::NEXT_STEP);
            loginState = LoginState::BOT_ON_LOGOUTQUEUE;
        }
        else if (loginState == LoginState::BOT_ON_LOGINQUEUE)
        {
            EmptyLoginSpace(space, FillStep::NEXT_STEP);
            loginState = LoginState::BOT_OFFLINE;
        }
    }
}

bool PlayerLoginInfo::IsQueued() const
{
    return loginState == LoginState::BOT_ON_LOGINQUEUE || loginState == LoginState::BOT_ON_LOGOUTQUEUE;
}

LoginCriterionFailType PlayerLoginInfo::MatchNoCriteria(LoginSpace const& space, LoginCriteria const& criteria, RealPlayerInfos const& realPlayers) const
{
    for (auto const& criterion : criteria)
    {
        if (criterion.second(*this, space, realPlayers))
            return criterion.first;
    }

    return LoginCriterionFailType::LOGIN_OK;
}

void PlayerLoginInfo::Update(Player* player)
{
    if (!player)
    {
        if (loginState == LoginState::BOT_ONLINE)
            loginState = LoginState::BOT_OFFLINE;
        return;
    }

    loginState = LoginState::BOT_ONLINE;
    level = player->GetLevel();
    position = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
    isNew = player->GetTotalPlayedTime() == 0;
    groupId = player->GetGroup() ? player->GetGroup()->GetGUID().GetCounter() : 0;
    guildId = player->GetGuildId();
}

void PlayerLoginInfo::MarkOffline()
{
    loginState = LoginState::BOT_OFFLINE;
    holder.reset();
    holderState = HolderState::HOLDER_EMPTY;
}

bool PlayerLoginInfo::LoginBot()
{
    if (loginState != LoginState::BOT_ON_LOGINQUEUE)
        return false;

    if (holderState != HolderState::HOLDER_RECEIVED)
        return false;

    uint32 nowMs = getMSTime();
    if (lastLoginAttempt && GetMSTimeDiffToNow(lastLoginAttempt) < loginAttemptDelayMs)
        return false;

    if (sRandomPlayerbotMgr->GetPlayerBot(GetGuid()))
    {
        loginState = LoginState::BOT_ONLINE;
        return false;
    }

    if (!holder)
    {
        holderState = HolderState::HOLDER_EMPTY;
        return false;
    }

    sRandomPlayerbotMgr->HandlePlayerBotLoginCallback(*holder);
    holder.reset();
    holderState = HolderState::HOLDER_EMPTY;

    lastLoginAttempt = nowMs;

    if (sPlayerbotAIConfig->randomBotTimedLogout)
    {
        sRandomPlayerbotMgr->SetValue(guid, "add", 1);
    }

    sRandomPlayerbotMgr->SetValue(guid, "logout", 0);

    Player* player = GetPlayer();
    if (!player)
    {
        loginState = LoginState::BOT_OFFLINE;
        return false;
    }

    Update(player);

    loginState = LoginState::BOT_ONLINE;
    return true;
}

bool PlayerLoginInfo::LogoutBot()
{
    if (loginState != LoginState::BOT_ON_LOGOUTQUEUE)
        return false;

    uint32 nowMs = getMSTime();
    if (lastLogoutAttempt && GetMSTimeDiffToNow(lastLogoutAttempt) < logoutAttemptDelayMs)
        return false;

    if (!sRandomPlayerbotMgr->GetPlayerBot(GetGuid()))
    {
        loginState = LoginState::BOT_OFFLINE;
        return false;
    }

    sRandomPlayerbotMgr->SetValue(guid, "add", 0);
    sRandomPlayerbotMgr->LogoutPlayerBot(GetGuid());
    lastLogoutAttempt = nowMs;

    if (sPlayerbotAIConfig->randomBotTimedOffline)
    {
        sRandomPlayerbotMgr->SetValue(guid, "logout", 1);
    }

    loginState = LoginState::BOT_OFFLINE;
    return true;
}

void PlayerBotLoginMgr::Update(RealPlayers& realPlayers)
{
    _queryProcessor.ProcessReadyCallbacks();

    if (sPlayerbotAIConfig->asyncBotLogin &&
        futureQueue.valid() &&
        futureQueue.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
    {
        return;
    }

    UpdateOnlineBots();

    if (botPool.empty())
    {
        if (!futurePool.valid())
            futurePool = std::async(std::launch::async, &PlayerBotLoginMgr::LoadBotsFromDb);
        else if (futurePool.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
        {
            botPool = futurePool.get();
            onlineBots.clear();
            for (auto& [guid, info] : botPool)
            {
                (void)guid;
                if (Player* player = info.GetPlayer())
                {
                    if (player->IsInWorld())
                    {
                        info.Update(player);
                        onlineBots.push_back(&info);
                    }
                }
            }
        }
        return;
    }

    if (!sPlayerbotAIConfig->asyncBotLogin)
    {
        BotInfos queue = FillLoginLogoutQueue(&botPool, realPlayers);
        if (!sPlayerbotAIConfig->preloadHolders)
            SendHolders(queue);
        else
            SendHolders(&botPool);

        if (!queue.empty())
            LoginLogoutBots(queue);
        return;
    }

    BotInfos queue;
    if (futureQueue.valid())
    {
        queue = futureQueue.get();
        if (!sPlayerbotAIConfig->preloadHolders)
            SendHolders(queue);
        else
            SendHolders(&botPool);
    }

    if (!queue.empty())
        LoginLogoutBots(queue);

    futureQueue = std::async(
        std::launch::async,
        [this, realPlayersSnapshot = realPlayers]()
        {
            return FillLoginLogoutQueue(&botPool, realPlayersSnapshot);
        });
}

BotPool PlayerBotLoginMgr::LoadBotsFromDb()
{
    BotPool newPool;
    QueryResult result = CharacterDatabase.Query(
        "SELECT c.account, c.guid, c.race, c.class, c.level, c.totaltime, c.map, c.position_x, c.position_y, c.position_z, c.orientation, "
        "(SELECT guildid FROM guild_member gm WHERE gm.guid = c.guid LIMIT 1) "
        "FROM characters c");
    if (!result)
        return newPool;

    do
    {
        Field* fields = result->Fetch();

        uint32 account = fields[0].GetUInt32();
        if (!sPlayerbotAIConfig->IsInRandomAccountList(account))
            continue;

        uint32 guid = fields[1].GetUInt32();
        uint8 race = fields[2].GetUInt8();
        uint8 cls = fields[3].GetUInt8();
        uint32 level = fields[4].GetUInt32();
        bool isNew = fields[5].GetUInt32() == 0;
        uint32 mapId = fields[6].GetUInt32();
        float x = fields[7].GetFloat();
        float y = fields[8].GetFloat();
        float z = fields[9].GetFloat();
        float o = fields[10].GetFloat();
        uint32 guildId = fields[11].IsNull() ? 0 : fields[11].GetUInt32();

        newPool.emplace(guid, PlayerLoginInfo(account, guid, race, cls, level, isNew, WorldLocation(mapId, x, y, z, o), guildId));
    } while (result->NextRow());

    return newPool;
}

void PlayerBotLoginMgr::SendHolders(BotInfos const& queue)
{
    uint32 const pingStart = getMSTime();
    _queryProcessor.AddCallback(CharacterDatabase.AsyncQuery("SELECT 1").WithCallback(
        [pingStart](QueryResult result)
        {
            RandomPlayerbotMgr::DatabasePing(std::move(result), pingStart, "CharacterDatabase");
        }));

    for (PlayerLoginInfo* info : queue)
    {
        if (sRandomPlayerbotMgr->GetDatabaseDelay("CharacterDatabase") > 100)
            break;

        if (info)
            info->SendHolder();
    }
}

void PlayerBotLoginMgr::SendHolders(BotPool* pool)
{
    if (!pool)
        return;

    uint32 const pingStart = getMSTime();
    _queryProcessor.AddCallback(CharacterDatabase.AsyncQuery("SELECT 1").WithCallback(
        [pingStart](QueryResult result)
        {
            RandomPlayerbotMgr::DatabasePing(std::move(result), pingStart, "CharacterDatabase");
        }));

    for (auto& [guid, info] : *pool)
    {
        (void)guid;
        if (sRandomPlayerbotMgr->GetDatabaseDelay("CharacterDatabase") > 100)
            break;

        info.SendHolder();
    }
}

void PlayerBotLoginMgr::UpdateOnlineBots()
{
    for (auto itr = onlineBots.begin(); itr != onlineBots.end();)
    {
        PlayerLoginInfo* info = *itr;
        if (!info)
        {
            itr = onlineBots.erase(itr);
            continue;
        }

        Player* player = info->GetPlayer();
        if (player && player->IsInWorld())
        {
            info->Update(player);
            ++itr;
            continue;
        }

        info->MarkOffline();
        itr = onlineBots.erase(itr);
    }
}

#define ADD_CRITERIA(type, condition) \
    criteria.push_back(std::make_pair(LoginCriterionFailType::type, \
        [](PlayerLoginInfo const& info, LoginSpace const& space, RealPlayerInfos const& realPlayers) { return (condition); }))
#define ADD_KEEP_CRITERIA(type, condition) \
    criteria.push_back(std::make_pair(LoginCriterionFailType::type, \
        [](PlayerLoginInfo const& info, LoginSpace const& space, RealPlayerInfos const& realPlayers) { return !(condition); }))

uint32 PlayerBotLoginMgr::GetLoginCriteriaSize()
{
    return static_cast<uint32>(sPlayerbotAIConfig->loginCriteria.size());
}

std::vector<std::string> PlayerBotLoginMgr::GetVariableLoginCriteria(uint8 attempt)
{
    if (attempt >= sPlayerbotAIConfig->loginCriteria.size())
        return {};

    return sPlayerbotAIConfig->loginCriteria[attempt];
}

LoginCriteria PlayerBotLoginMgr::GetLoginCriteria(uint8 attempt)
{
    LoginCriteria criteria;

    std::vector<std::string> configCriteria = sPlayerbotAIConfig->defaultLoginCriteria;
    std::vector<std::string> attemptCriteria = GetVariableLoginCriteria(attempt);
    configCriteria.insert(configCriteria.end(), attemptCriteria.begin(), attemptCriteria.end());

    for (std::string const& criterion : configCriteria)
    {
        if (criterion == "maxbots")
            ADD_CRITERIA(MAX_BOTS, space.totalSpace <= 0);
        else if (criterion == "spareroom" && attempt > 0)
            ADD_CRITERIA(SPARE_ROOM, space.totalSpace <= static_cast<int32>(sPlayerbotAIConfig->freeRoomForNonSpareBots));
        else if (criterion == "online")
            ADD_KEEP_CRITERIA(ONLINE, info.IsOnline());
        else if (criterion == "logoff" && sPlayerbotAIConfig->randomBotTimedLogout)
            ADD_CRITERIA(RANDOM_TIMED_LOGOUT, info.IsOnline() && !sRandomPlayerbotMgr->GetValue(info.GetId(), "add"));
        else if (criterion == "offline" && sPlayerbotAIConfig->randomBotTimedOffline)
            ADD_CRITERIA(RANDOM_TIMED_OFFLINE, !info.IsOnline() && sRandomPlayerbotMgr->GetValue(info.GetId(), "logout"));
        else if (criterion == "bg")
            ADD_KEEP_CRITERIA(BG, info.IsOnline() && info.IsInBG());
        else if (criterion == "arena")
            ADD_KEEP_CRITERIA(ARENA, info.IsOnline() && info.IsInArena());
        else if (criterion == "instance")
            ADD_KEEP_CRITERIA(INSTANCE, info.IsOnline() && info.IsInInstance());
        else if (criterion == "classrace")
            ADD_CRITERIA(CLASSRACE,
                (info.GetClass() < MAX_CLASSES && info.GetRace() < MAX_RACES) ?
                (space.classRaceBucket[info.GetClass()][info.GetRace()] <= 0) : true);
        else if (criterion == "level")
            ADD_CRITERIA(LEVEL, (info.GetLevel() <= DEFAULT_MAX_LEVEL) ? (space.levelBucket[info.GetLevel()] <= 0) : true);
        else if (criterion == "range")
            ADD_KEEP_CRITERIA(RANGE, info.IsNearPlayer(realPlayers));
        else if (criterion == "map")
            ADD_KEEP_CRITERIA(MAP, info.IsOnPlayerMap(realPlayers));
        else if (criterion == "guild")
            ADD_KEEP_CRITERIA(GUILD, info.IsInPlayerGuild(realPlayers));
        else if (criterion == "group")
            ADD_KEEP_CRITERIA(GROUP, info.IsInPlayerGroup(realPlayers));
    }

    return criteria;
}

bool PlayerBotLoginMgr::CriteriaStillValid(LoginCriterionFailType oldFailType, LoginCriteria const& criteria)
{
    if (oldFailType == LoginCriterionFailType::UNKNOWN ||
        oldFailType == LoginCriterionFailType::MAX_BOTS ||
        oldFailType == LoginCriterionFailType::SPARE_ROOM ||
        oldFailType == LoginCriterionFailType::CLASSRACE ||
        oldFailType == LoginCriterionFailType::LEVEL)
    {
        return false;
    }

    for (auto const& criterion : criteria)
        if (criterion.first == oldFailType)
            return true;

    return false;
}

RealPlayerInfos PlayerBotLoginMgr::GetPlayerInfos(RealPlayers const& realPlayers)
{
    RealPlayerInfos infos;
    infos.reserve(realPlayers.size());
    for (auto const& [guid, player] : realPlayers)
    {
        (void)guid;
        if (player)
            infos.emplace_back(player);
    }
    return infos;
}

void PlayerBotLoginMgr::FillLoginSpace(BotPool* pool, LoginSpace& space, FillStep step)
{
    if (!pool)
        return;

    space.currentSpace = static_cast<int32>(GetMaxOnlineBotCount());
    space.totalSpace = static_cast<int32>(GetMaxOnlineBotCount());

    for (uint32 level = 1; level <= DEFAULT_MAX_LEVEL; ++level)
        space.levelBucket[level] = static_cast<int32>(GetLevelBucketSize(level));

    for (uint32 race = 1; race < MAX_RACES; ++race)
        for (uint32 cls = 1; cls < MAX_CLASSES; ++cls)
            space.classRaceBucket[cls][race] = static_cast<int32>(GetClassRaceBucketSize(cls, race));

    for (auto& [guid, botInfo] : *pool)
    {
        (void)guid;
        botInfo.FillLoginSpace(space, step);

        if (botInfo.GetLoginState() == LoginState::BOT_ONLINE || botInfo.GetLoginState() == LoginState::BOT_ON_LOGOUTQUEUE)
            space.currentSpace--;
    }
}

BotInfos PlayerBotLoginMgr::FillLoginLogoutQueue(BotPool* pool, RealPlayers const& realPlayers)
{
    BotInfos queue;
    if (!pool)
        return queue;

    LoginSpace loginSpace;
    RealPlayerInfos playerInfos = GetPlayerInfos(realPlayers);
    FillLoginSpace(pool, loginSpace, FillStep::NEXT_STEP);

    std::unordered_map<uint32, LoginCriterionFailType> loginFails;
    std::set<PlayerLoginInfo*> potentialQueue;

    if (sPlayerBotLoginMgr->debug)
        TC_LOG_INFO("playerbots", "PlayerBotLoginMgr: Initial space %d", loginSpace.totalSpace);

    for (uint8 attempt = 0; attempt <= GetLoginCriteriaSize(); ++attempt)
    {
        LoginCriteria criteria = GetLoginCriteria(attempt);

        for (auto& [guid, botInfo] : *pool)
        {
            if (CriteriaStillValid(loginFails[guid], criteria))
                continue;

            botInfo.EmptyLoginSpace(loginSpace, FillStep::NOW);
            loginFails[guid] = botInfo.MatchNoCriteria(loginSpace, criteria, playerInfos);
            botInfo.FillLoginSpace(loginSpace, FillStep::NOW);

            bool const wantOnlineFromEvent = sRandomPlayerbotMgr->GetValue(guid, "add") != 0;
            bool const isWanted = wantOnlineFromEvent && loginFails[guid] == LoginCriterionFailType::LOGIN_OK;

            if (!isWanted && attempt)
                continue;

            botInfo.SetQueue(isWanted, loginSpace);
            if (botInfo.IsQueued())
                potentialQueue.insert(&botInfo);

            if (attempt && loginSpace.totalSpace <= 0)
                break;
        }

        if (sPlayerBotLoginMgr->debug)
        {
            std::string variableCriteria;
            for (std::string const& criterion : GetVariableLoginCriteria(attempt))
            {
                if (!variableCriteria.empty())
                    variableCriteria += ",";
                variableCriteria += criterion;
            }

            TC_LOG_INFO("playerbots", "PlayerBotLoginMgr: Attempt %u (%s), space left %d",
                attempt, variableCriteria.c_str(), loginSpace.totalSpace);
        }

        if (loginSpace.totalSpace <= 0)
            break;
    }

    uint32 queuedLogins = 0;
    for (PlayerLoginInfo* info : potentialQueue)
    {
        if (!info)
            continue;
        if (info->GetLoginState() != LoginState::BOT_ON_LOGINQUEUE)
            continue;
        if (queuedLogins >= sPlayerbotAIConfig->randomBotsMaxLoginsPerInterval)
            break;

        queue.push_back(info);
        queuedLogins++;
    }

    loginSpace.currentSpace -= queuedLogins;

    uint32 maxLogouts = 0;
    if (loginSpace.currentSpace < static_cast<int32>(sPlayerbotAIConfig->freeRoomForNonSpareBots))
        maxLogouts = sPlayerbotAIConfig->freeRoomForNonSpareBots - loginSpace.currentSpace;

    uint32 queuedLogouts = 0;
    for (PlayerLoginInfo* info : potentialQueue)
    {
        if (!info)
            continue;
        if (info->GetLoginState() != LoginState::BOT_ON_LOGOUTQUEUE)
            continue;
        if (queuedLogouts >= maxLogouts)
            break;

        queue.push_back(info);
        queuedLogouts++;
    }

    if (sPlayerBotLoginMgr->debug)
    {
        TC_LOG_INFO("playerbots", "PlayerBotLoginMgr: Queued to log in: %u, out: %u",
            queuedLogins, queuedLogouts);
    }

    return queue;
}

void PlayerBotLoginMgr::LoginLogoutBots(BotInfos const& queue)
{
    for (PlayerLoginInfo* info : queue)
    {
        if (!info)
            continue;

        if (info->LoginBot())
        {
            if (std::find(onlineBots.begin(), onlineBots.end(), info) == onlineBots.end())
                onlineBots.push_back(info);
        }
        if (info->LogoutBot())
        {
            onlineBots.erase(std::remove(onlineBots.begin(), onlineBots.end(), info), onlineBots.end());
        }
    }
}

uint32 PlayerBotLoginMgr::GetMaxLevel()
{
    return std::max(
        sPlayerbotAIConfig->randomBotMinLevel,
        std::min(
            sPlayerbotAIConfig->randomBotMaxLevel,
            sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)));
}

uint32 PlayerBotLoginMgr::GetMaxOnlineBotCount()
{
    return sRandomPlayerbotMgr->GetValue(uint32(0), "bot_count");
}

uint32 PlayerBotLoginMgr::GetClassRaceBucketSize(uint8 cls, uint8 race)
{
    if (cls >= MAX_CLASSES || race >= MAX_RACES)
        return 0;

    uint32 prob = sPlayerbotAIConfig->classRaceProbability[cls][race];
    if (!prob)
        return 0;

    if (sPlayerbotAIConfig->useFixedClassRaceCounts)
        return prob;

    if (!sPlayerbotAIConfig->classRaceProbabilityTotal)
        return GetMaxOnlineBotCount();

    return std::max<uint32>(1, GetMaxOnlineBotCount() * prob / sPlayerbotAIConfig->classRaceProbabilityTotal);
}

uint32 PlayerBotLoginMgr::GetLevelBucketSize(uint32 level)
{
    if (!level || level > GetMaxLevel() || level > DEFAULT_MAX_LEVEL)
        return 0;

    uint32 prob = sPlayerbotAIConfig->levelProbability[level];
    if (!prob)
        return 0;

    uint32 levelProbabilityTotal = 0;
    for (uint32 l = 1; l <= GetMaxLevel() && l <= DEFAULT_MAX_LEVEL; ++l)
        levelProbabilityTotal += sPlayerbotAIConfig->levelProbability[l];

    if (!levelProbabilityTotal)
        return 0;

    return std::max<uint32>(1, GetMaxOnlineBotCount() * prob / levelProbabilityTotal);
}
