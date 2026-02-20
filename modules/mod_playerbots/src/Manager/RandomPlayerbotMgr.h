#ifndef _PLAYERBOT_RANDOMPLAYERBOTMGR_H
#define _PLAYERBOT_RANDOMPLAYERBOTMGR_H

#include "AsyncCallbackProcessor.h"
#include "PlayerbotMgr.h"
#include "DatabaseEnvFwd.h"
#include <atomic>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class CachedEvent
{
public:
    CachedEvent() : value(0), lastChangeTime(0), validIn(0), data("") {}
    CachedEvent(const CachedEvent& other)
        : value(other.value), lastChangeTime(other.lastChangeTime), validIn(other.validIn), data(other.data)
    {
    }
    CachedEvent(uint32 value, uint32 lastChangeTime, uint32 validIn, std::string const data = "")
        : value(value), lastChangeTime(lastChangeTime), validIn(validIn), data(data)
    {
    }

    bool IsEmpty() { return !lastChangeTime; }

public:
    uint32 value;
    uint32 lastChangeTime;
    uint32 validIn;
    std::string data;
};

class BotEventCache
{
public:
    bool loaded = false;
    std::unordered_map<std::string, CachedEvent> events;
};

class Player;
class ObjectGuid;
class WorldLocation;
class Item;

class botPIDImpl;
class botPID
{
public:
    botPID(double dt, double max, double min, double Kp, double Ki, double Kd);
    botPID(botPID const&) = delete;
    botPID& operator=(botPID const&) = delete;
    void adjust(double Kp, double Ki, double Kd);
    void reset();
    double calculate(double setpoint, double pv);
    ~botPID();

private:
    botPIDImpl* pimpl;
};

class RandomPlayerbotMgr : public PlayerbotHolder
{
public:
    struct BattlegroundInfo
    {
        std::vector<uint32> bgInstances;
        std::vector<uint32> ratedArenaInstances;
        std::vector<uint32> skirmishArenaInstances;
        uint32 bgInstanceCount = 0;
        uint32 ratedArenaInstanceCount = 0;
        uint32 skirmishArenaInstanceCount = 0;
        uint32 minLevel = 0;
        uint32 maxLevel = 0;
        uint32 activeBgQueue = 0;
        uint32 activeRatedArenaQueue = 0;
        uint32 activeSkirmishArenaQueue = 0;
        uint32 bgHordeBotCount = 0;
        uint32 bgAllianceBotCount = 0;
        uint32 bgHordePlayerCount = 0;
        uint32 bgAlliancePlayerCount = 0;
        uint32 ratedArenaBotCount = 0;
        uint32 ratedArenaPlayerCount = 0;
        uint32 skirmishArenaBotCount = 0;
        uint32 skirmishArenaPlayerCount = 0;
    };

    struct farm_spot
    {
        uint32 min_level;
        uint32 max_level;
        uint32 map_id;
        float x;
        float y;
        float z;
    };

    struct farm_zone
    {
        uint32 zone_id;
        uint32 zone_type;
        uint32 min_level;
        uint32 max_level;
        Team team_disabled;
        uint32 map_id;
        uint32 min_player;
        uint32 max_player;

        std::vector<farm_spot> spots;
    };

    struct city
    {
        uint32 zone_id;
        uint32 city_id;
        uint32 map_id;
        uint32 min_level;
        uint32 max_level;
        Team team_disabled;
        float x;
        float y;
        float z;
    };

public:
    RandomPlayerbotMgr();
    virtual ~RandomPlayerbotMgr();
    static RandomPlayerbotMgr* instance()
    {
        static RandomPlayerbotMgr instance;
        return &instance;
    }

    /// <summary>
    /// Allow pre load reserve on internal containers
    /// </summary>
    /// <param name="size"></param>
    void Reserve(const uint32 size);
    void UpdateAIInternal(uint32 elapsed, bool minimal = false) override;

public:
    bool IsRandomBot(Player* bot);
    bool IsRandomBot(ObjectGuid::LowType bot);
    static void DatabasePing(QueryResult result, uint32 pingStart, std::string const& db);

    void TagForRandomize(Player* bot, uint32 level);
    void RandomizeFirst(Player* bot);
    void Randomize(Player* bot);
    
    void Clear(Player* bot);
    
    void OnPlayerLogout(Player* player);
    void OnPlayerLogin(Player* player);
    void OnPlayerLoginError(uint32 bot);
    
    Player* GetRandomPlayer();
    Player* GetPlayer(uint32 playerGuid);
    std::vector<Player*> GetPlayers()
    {
        std::lock_guard<std::mutex> lock(_playersMutex);
        return _players;
    };
    bool HasPlayers() const
    {
        return _realPlayersCount.load(std::memory_order_relaxed) > 0;
    }
    PlayerBotMap GetAllBots() { return GetPlayerBotsSnapshot(); };
    void Refresh(Player* bot);
    uint32 GetMaxAllowedBotCount();
    bool AddRandomBot(uint32 bot);
    bool ProcessBot(Player* player);
    void Revive(Player* player);
    void ChangeStrategy(Player* player);
    void InstaRandomize(Player* bot);
    void UpdateGearSpells(Player* bot);
    uint32 GetValue(Player* bot, std::string const type);
    uint32 GetValue(uint32 bot, std::string const type);
    int32 GetValueValidTime(uint32 bot, std::string const event);
    std::string const GetData(uint32 bot, std::string const type);
    void SetValue(uint32 bot, std::string const type, uint32 value, std::string const data = "");
    void SetValue(Player* bot, std::string const type, uint32 value, std::string const data = "");
    void Remove(Player* bot);
    void SaveCurTime();
    void SyncEventTimers();
    void CheckPlayers();
    void CheckBgQueue();
    void CheckLfgQueue();
    void AddOfflineGroupBots();
    void LoginFreeBots();
    static Item* CreateTempItem(uint32 item, uint32 count, Player const* player, uint32 randomPropertyId = 0);
    static InventoryResult CanEquipUnseenItem(Player* player, uint8 slot, uint16& dest, uint32 item);
    void LoadNamedLocations();
    bool AddNamedLocation(std::string const& name, WorldLocation const& location);
    bool GetNamedLocation(std::string const& name, WorldLocation& location);
    uint32 GetZoneLevel(uint16 mapId, float teleX, float teleY, float teleZ);
    void PrintTeleportCache();
    void PrintStats(uint32 requesterGuid);
    double GetBuyMultiplier(Player* bot);
    double GetSellMultiplier(Player* bot);
    void HandleCommand(uint32 type, std::string const& text, Player& fromPlayer, std::string channelName,
        Team team, uint32 lang);
    bool HandlePlayerbotConsoleCommand(ChatHandler* handler, char const* args);
    std::string HandleRemoteCommand(std::string request);
    void Hotfix(Player* player, uint32 version);
    void LogPlayerLocation();
    void DelayedFacingFix();
    void MirrorAh();
    void MovePlayerBot(uint32 guid, PlayerbotHolder* newHolder);
    std::vector<uint32> GetBgBots(uint32 bracket);
    void SetDatabaseDelay(std::string const& db, uint32 delay);
    uint32 GetDatabaseDelay(std::string const& db) const;
    float getActivityMod()
    {
        std::lock_guard<std::recursive_mutex> lock(_stateMutex);
        return _activityMod;
    }
    float getActivityPercentage()
    {
        std::lock_guard<std::recursive_mutex> lock(_stateMutex);
        return _activityMod * 100.0f;
    }
    void setActivityPercentage(float percentage)
    {
        std::lock_guard<std::recursive_mutex> lock(_stateMutex);
        _activityMod = percentage / 100.0f;
    }
    static uint8 GetTeamClassIdx(bool isAlliance, uint8 claz) { return isAlliance * 20 + claz; }

    void ScheduleTeleport(uint32 bot, uint32 time = 0);
    void PrepareAddclassCache();
    std::map<uint8, std::vector<ObjectGuid>>& AddclassCache() { return _addclassCache; };
    void PrepareTeleportCache();
    void RandomTeleportForLevel(Player* bot);
    void RandomTeleport(Player* bot);
    void RandomTeleportForRpg(Player* bot, bool activeOnly = true);
    std::map<uint32, std::map<uint32, BattlegroundInfo>> BattlegroundData;
    std::map<TeamId, std::vector<uint32>> LfgDungeons;

protected:
    void OnBotLoginInternal(Player* const bot) override;

private:
    typedef std::unordered_map<uint32, std::list<float>> botPerformanceMetric;

    void GetBots();
    void RefreshRandomBotGuidCache();
    uint32 AddRandomBots();
    bool ProcessBot(uint32 bot);
    void ScaleBotActivity();
    void QueueDatabasePingQuery();
    void EnsureRandomBotGuilds();
    void ScheduleRandomize(uint32 bot, uint32 time);
    std::vector<std::pair<uint32, uint32>> RpgLocationsNear(WorldLocation pos,
        std::map<uint32, std::map<uint32, std::vector<std::string>>> const& areaNames, uint32 radius = 2000);
    farm_spot const* GetFarmZoneForPlayer(Player* player);
    city const* GetCityForPlayer(Player* player);
    void ScheduleChangeStrategy(uint32 bot, uint32 time = 0);
    CachedEvent* FindEvent(uint32 bot, std::string const& event);
    uint32 GetEventValue(uint32 bot, std::string const event);
    std::string const GetEventData(uint32 bot, std::string const event);
    uint32 SetEventValue(uint32 bot, std::string const event, uint32 value, uint32 validIn, std::string const data = "");
    void PushMetric(botPerformanceMetric& metric, uint32 bot, float value, uint32 maxNum = 60) const;
    float GetMetricDelta(botPerformanceMetric& metric) const;

public:
    uint32 activeBots = 0;
private:
    // PID values are set in constructor.
    botPID pid = botPID(1, 50, -50, 0, 0, 0);
    float _activityMod = 0.25;
    bool _isBotInitializing = true;
    time_t _playersCheckTimer;
    typedef void (RandomPlayerbotMgr::* ConsoleCommandHandler)(Player*);
    std::vector<Player*> _players;
    std::atomic<uint32> _realPlayersCount{ 0 };
    mutable std::mutex _playersMutex;
    uint32 _processTicks;
    uint32 _playersLevel;
    time_t _bgCheckTimer = 0;
    time_t _lfgCheckTimer = 0;
    time_t _offlineGroupBotsTimer = 0;
    time_t _logPlayerLocationTimer = 0;
    time_t _delayedFacingFixTimer = 0;
    time_t _eventTimeSyncTimer = 0;
    bool _eventTimersSynced = false;
    bool _randomBotGuildsInitialized = false;
    time_t _randomBotGuildsLastUpdate = 0;
    // -- lists
    std::unordered_map<uint32, BotEventCache> _eventCache;
    std::map<std::string, uint32> _databaseDelay;
    std::unordered_map<std::string, WorldLocation> _namedLocations;
    std::map<uint32, std::map<uint32, std::vector<WorldLocation>>> _rpgLocsCacheLevel;
    std::map<uint32, std::map<uint32, std::vector<std::string>>> _rpgAreaNames;
    std::map<uint8, std::vector<ObjectGuid>> _addclassCache;
    std::vector<city> _city_cache_data;
    std::vector<farm_zone> _farm_cache_data;

    QueryCallbackProcessor _queryProcessor;
    float _avgUpdateTime = 0.0f;
    uint32 _dbPingTimer = 0;
    uint32 _randomBotGuidCacheTimer = 0;
    size_t _randomBotGuidCursor = 0;
    std::vector<uint32> _randomBotGuidCache;
    std::vector<uint32> _availableBots;
    std::unordered_map<std::string, botPerformanceMetric> _botPerformanceMetrics;
    std::unordered_set<uint32> _currentBotSet;
    std::vector<uint32> _currentBots;
    mutable std::recursive_mutex _stateMutex;
};

#define sRandomPlayerbotMgr RandomPlayerbotMgr::instance()

#endif
