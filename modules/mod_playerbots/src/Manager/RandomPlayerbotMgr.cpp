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

#include "RandomPlayerbotMgr.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <limits>
#include <random>
#include <sstream>
#include <unordered_set>

#include "AccountMgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BotFactory.h"
#include "CharacterCache.h"
#include "RandomPlayerbotFactory.h"
#include "CellImpl.h"
#include "Chat.h"
#include "ChannelMgr.h"
#include "DBCStores.h"
#include "DBCStructure.h"
#include "DatabaseEnv.h"
#include "Define.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "GuildMgr.h"
#include "Helper.h"
#include "IVMapManager.h"
#include "Item.h"
#include "LFGMgr.h"
#include "MapManager.h"
#include "MemoryMonitor.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotLoginMgr.h"
#include "PerformanceMonitor.h"
#include "Playerbots.h"
#include "RandomItemManager.h"
#include "TravelMgr.h"
#include "Group.h"
#include "SharedDefines.h"
#include "Unit.h"
#include "World.h"
#include "ObjectMgr.h"

namespace
{
    uint32 constexpr GM_ISLAND_ZONE_ID = 876;

    bool IsHostileForTeam(AreaTableEntry const* area, uint32 team)
    {
        if (!area)
            return false;

        switch (area->team)
        {
            case AREATEAM_ALLY:
                return team != ALLIANCE;
            case AREATEAM_HORDE:
                return team != HORDE;
            default:
                return false;
        }
    }

    bool IsInvalidTeleportAreaForBot(Player const* bot, uint32 zoneId, uint32 areaId)
    {
        if (!bot)
            return true;

        if (IsHostileForTeam(sAreaTableStore.LookupEntry(areaId), bot->GetTeam()) ||
            IsHostileForTeam(sAreaTableStore.LookupEntry(zoneId), bot->GetTeam()))
            return true;

        return sPlayerbotAIConfig->IsPvpProhibited(zoneId, areaId);
    }
}

class botPIDImpl
{
public:
    botPIDImpl(double dt, double max, double min, double Kp, double Ki, double Kd)
        : _dt(dt), _max(max), _min(min), _Kp(Kp), _Ki(Ki), _Kd(Kd), _pre_error(0.0), _integral(0.0)
    {
    }

    double calculate(double setpoint, double pv)
    {
        double const error = setpoint - pv;
        double const Pout = _Kp * error;
        _integral += error * _dt;
        double const Iout = _Ki * _integral;
        double const derivative = (error - _pre_error) / _dt;
        double const Dout = _Kd * derivative;
        double output = Pout + Iout + Dout;

        if (output > _max)
        {
            output = _max;
            _integral -= error * _dt;
        }
        else if (output < _min)
        {
            output = _min;
            _integral -= error * _dt;
        }

        _pre_error = error;
        return output;
    }

    void adjust(double Kp, double Ki, double Kd)
    {
        _Kp = Kp;
        _Ki = Ki;
        _Kd = Kd;
    }

    void reset() { _integral = 0.0; }

private:
    double _dt;
    double _max;
    double _min;
    double _Kp;
    double _Ki;
    double _Kd;
    double _pre_error;
    double _integral;
};

botPID::botPID(double dt, double max, double min, double Kp, double Ki, double Kd)
{
    pimpl = new botPIDImpl(dt, max, min, Kp, Ki, Kd);
}

void botPID::adjust(double Kp, double Ki, double Kd)
{
    pimpl->adjust(Kp, Ki, Kd);
}

void botPID::reset()
{
    pimpl->reset();
}

double botPID::calculate(double setpoint, double pv)
{
    return pimpl->calculate(setpoint, pv);
}

botPID::~botPID()
{
    delete pimpl;
}

RandomPlayerbotMgr::RandomPlayerbotMgr()
    : PlayerbotHolder(),
    _processTicks(0)
{
    _playersLevel = 1;// sPlayerbotAIConfig->randombotStartingLevel;
    _playersCheckTimer = 0;
    _dbPingTimer = 0;
    pid.adjust(0.05, 0.001, 0.05);
}

RandomPlayerbotMgr::~RandomPlayerbotMgr() {}

void RandomPlayerbotMgr::Reserve(const uint32 size)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    _currentBots.reserve(size);
    _availableBots.reserve(size);
    _currentBotSet.reserve(size);
}

uint32 RandomPlayerbotMgr::GetMaxAllowedBotCount()
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    return GetEventValue(0, "bot_count");
}

void RandomPlayerbotMgr::ScheduleRandomize(uint32 bot, uint32 time)
{
    SetEventValue(bot, "randomize", 1, time);
}

void RandomPlayerbotMgr::ScheduleTeleport(uint32 bot, uint32 time)
{
    if (!time)
        time = 60 + urand(sPlayerbotAIConfig->randomBotUpdateInterval, sPlayerbotAIConfig->randomBotUpdateInterval * 3);

    SetEventValue(bot, "teleport", 1, time);
}

void RandomPlayerbotMgr::ScheduleChangeStrategy(uint32 bot, uint32 time)
{
    if (!time)
        time = urand(sPlayerbotAIConfig->minRandomBotReviveTime, sPlayerbotAIConfig->maxRandomBotReviveTime);

    SetEventValue(bot, "change_strategy", 1, time);
}

void RandomPlayerbotMgr::UpdateAIInternal(uint32 elapsed, bool /*minimal*/)
{
#ifdef MEMORY_MONITOR
    sMemoryMonitor.Print();
    if (sPlayerbotAIConfig->hasLog("memory.csv"))
        sMemoryMonitor.LogCount("memory.csv");
#endif

    _queryProcessor.ProcessReadyCallbacks();

    totalPmo.reset();
    totalPmo = sPerformanceMonitor->start(PERF_MON_TOTAL, "RandomPlayerbotMgr::FullTick");

    if (!sPlayerbotAIConfig->randomBotAutologin || !sPlayerbotAIConfig->enabled)
        return;

    if (_processTicks == 0)
        LoadNamedLocations();
    ++_processTicks;

    if (!_eventTimersSynced)
    {
        SyncEventTimers();
        _eventTimersSynced = true;
    }

    if (!_eventTimeSyncTimer || time(nullptr) > (_eventTimeSyncTimer + 60))
        SaveCurTime();

    if (sPlayerbotAIConfig->syncLevelWithPlayers)
        CheckPlayers();

    ScaleBotActivity();

    std::vector<Player*> const realPlayersSnapshot = GetPlayers();
    bool const hasRealPlayers = !realPlayersSnapshot.empty();

    if (sPlayerbotAIConfig->asyncBotLogin)
    {
        auto pmo = sPerformanceMonitor->start(PERF_MON_RNDBOT, "AsyncBotLogin");
        RealPlayers realPlayers;
        for (Player* player : realPlayersSnapshot)
        {
            if (!player || !player->IsInWorld())
                continue;

            PlayerbotAI* playerAI = GET_PLAYERBOT_AI(player);
            if (!playerAI || playerAI->IsRealPlayer())
                realPlayers[player->GetGUID().GetCounter()] = player;
        }

        sPlayerBotLoginMgr->Update(realPlayers);

        if (pmo)
            pmo->finish();
    }

    uint32 const nowMs = getMSTime();
    if (sPlayerbotAIConfig->randomBotsDatabasePingInterval &&
        GetMSTimeDiffToNow(_dbPingTimer) >= sPlayerbotAIConfig->randomBotsDatabasePingInterval)
    {
        QueueDatabasePingQuery();
        _dbPingTimer = nowMs;
    }

    uint32 maxAllowedBotCount = GetEventValue(0, "bot_count");
    if (!maxAllowedBotCount || (maxAllowedBotCount < sPlayerbotAIConfig->minRandomBots ||
        maxAllowedBotCount > sPlayerbotAIConfig->maxRandomBots))
    {
        maxAllowedBotCount = sPlayerbotAIConfig->minRandomBots + (std::rand() % (sPlayerbotAIConfig->maxRandomBots - sPlayerbotAIConfig->minRandomBots + 1));
        auto randomInterval = sPlayerbotAIConfig->randomBotCountChangeMinInterval +
            (std::rand() % (sPlayerbotAIConfig->randomBotCountChangeMaxInterval - sPlayerbotAIConfig->randomBotCountChangeMinInterval + 1));
        SetEventValue(0, "bot_count", maxAllowedBotCount, randomInterval);
    }

    GetBots();
    if (sPlayerbotAIConfig->randomBotJoinBG && hasRealPlayers)
        CheckBgQueue();
    if (sPlayerbotAIConfig->randomBotJoinLfg && hasRealPlayers)
        CheckLfgQueue();
    if (hasRealPlayers)
        AddOfflineGroupBots();
    std::vector<uint32> availableBots;
    uint32 availableBotCount = 0;
    {
        std::lock_guard<std::recursive_mutex> stateLock(_stateMutex);
        _availableBots = _currentBots;
        availableBots = _availableBots;
        availableBotCount = static_cast<uint32>(_currentBots.size());
    }
    uint32 onlineBotCount = GetPlayerbotsCount();
    bool const loginAllowedByRealPlayers = !(sPlayerbotAIConfig->randomBotLoginWithPlayer && !hasRealPlayers);

    EnsureRandomBotGuilds();

    uint32 onlineBotFocus = 75;
    if (onlineBotCount < (uint32)(sPlayerbotAIConfig->minRandomBots * 90 / 100))
        onlineBotFocus = 25;

    // only keep updating till initializing time has completed,
    // which prevents unneeded expensive GameTime calls.
    if (_isBotInitializing)
    {
        _isBotInitializing = sWorld->GetUptime() < sPlayerbotAIConfig->maxRandomBots * (0.11 + 0.4);
    }

    uint32 const updateIntervalMs = std::max<uint32>(1, sPlayerbotAIConfig->randomBotUpdateInterval);
    SetNextCheckDelay(updateIntervalMs);

    auto pmo = sPerformanceMonitor->start(
            PERF_MON_TOTAL,
            onlineBotCount < maxAllowedBotCount ? "RandomPlayerbotMgr::Login" : "RandomPlayerbotMgr::UpdateAIInternal");

    if (availableBotCount < maxAllowedBotCount && loginAllowedByRealPlayers && !sWorld->IsShuttingDown())
    {
        AddRandomBots();
    }

    uint32 updateBots = sPlayerbotAIConfig->randomBotsPerInterval * onlineBotFocus / 100;
    uint32 maxNewBots = onlineBotCount < maxAllowedBotCount ? maxAllowedBotCount - onlineBotCount : 0;
    uint32 loginBots = std::min(sPlayerbotAIConfig->randomBotsPerInterval - updateBots, maxNewBots);
    uint32 const dbDelay = GetDatabaseDelay("CharacterDatabase");
    uint32 const maxLoginsPerInterval = sPlayerbotAIConfig->randomBotsMaxLoginsPerInterval;
    uint32 const pendingLogins = static_cast<uint32>(GetBotLoadingCount());

    if (maxLoginsPerInterval == 0)
        loginBots = 0;
    else
    {
        loginBots = std::min(loginBots, maxLoginsPerInterval);
        if (pendingLogins >= maxLoginsPerInterval)
            loginBots = 0;
        else
            loginBots = std::min(loginBots, maxLoginsPerInterval - pendingLogins);
    }

    if (sPlayerbotAIConfig->asyncBotLogin)
    {
        loginBots = 0;
    }
    else if (sPlayerbotAIConfig->randomBotsDatabaseDelayThreshold &&
        dbDelay > sPlayerbotAIConfig->randomBotsDatabaseDelayThreshold)
    {
        loginBots = 0;
    }

    if (!loginAllowedByRealPlayers || sWorld->IsShuttingDown())
    {
        loginBots = 0;
    }

    if (!availableBots.empty())
    {
        // Update bots
        for (auto bot : availableBots)
        {
            if (GetPlayerBot(bot))
            {
                if (ProcessBot(bot))
                    updateBots--;

                if (!updateBots)
                    break;
            }
        }

        if (loginBots)
        {
            loginBots += updateBots;
            loginBots = std::min(loginBots, maxNewBots);
            if (maxLoginsPerInterval > pendingLogins)
                loginBots = std::min(loginBots, maxLoginsPerInterval - pendingLogins);
            else
                loginBots = 0;

            TC_LOG_INFO("playerbots", "%d new bots", loginBots);

            // Log in bots
            for (auto bot : availableBots)
            {
                if (GetPlayerBot(bot))
                    continue;

                if (ProcessBot(bot))
                {
                    loginBots--;
                }

                if (!loginBots)
                    break;
            }
        }
    }

    if (hasRealPlayers)
        LoginFreeBots();

    LogPlayerLocation();
    DelayedFacingFix();
    MirrorAh();

    sMapMgr->DoForAllMaps(
        [](Map* map)
        {
            if (map)
                sPerformanceMonitor->Init(map->GetId(), map->GetInstanceId());
        });

    if (pmo)
        pmo->finish();
}

namespace
{
    static bool EndsWith(std::string const& s, std::string const& suffix)
    {
        return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    static std::string FormatMoney(uint64 copper)
    {
        uint64 gold = copper / GOLD;
        copper %= GOLD;
        uint64 silver = copper / SILVER;
        copper %= SILVER;

        std::ostringstream out;
        bool first = true;
        if (gold)
        {
            out << gold << "g";
            first = false;
        }
        if (silver || (!gold && (copper == 0)))
        {
            if (!first) out << " ";
            out << silver << "s";
            first = false;
        }
        if (copper)
        {
            if (!first) out << " ";
            out << copper << "c";
        }
        return out.str();
    }

    static bool PickRandomKillTarget(uint32 targetLevel, uint32 levelWindow, uint32& outEntry, std::string& outName)
    {
        outEntry = 0;
        outName.clear();

        CreatureTemplateContainer const* ctc = sObjectMgr->GetCreatureTemplates();
        if (!ctc)
            return false;

        uint32 count = 0;
        for (CreatureTemplateContainer::const_iterator itr = ctc->begin(); itr != ctc->end(); ++itr)
        {
            CreatureTemplate const& c = itr->second;

            // Skip non-hostile/non-combat entries best-effort.
            if (c.npcflag != 0 || c.npcflag2 != 0)
                continue;
            if (c.rank >= 3) // world bosses etc
                continue;
            if (c.minlevel < 1 || c.maxlevel < 1)
                continue;
            if (c.type == CREATURE_TYPE_CRITTER)
                continue;

            uint32 minL = c.minlevel;
            uint32 maxL = c.maxlevel;
            if (targetLevel < minL || targetLevel > maxL)
                continue;

            // Ensure it isn't too wide; prefer more "normal" mobs.
            if ((maxL - minL) > 10)
                continue;

            // Tighten to a window around targetLevel.
            if (targetLevel > levelWindow && minL + levelWindow < targetLevel)
                continue;
            if (maxL > targetLevel + levelWindow)
                continue;

            ++count;
            if (urand(1, count) == 1)
            {
                outEntry = c.Entry;
                outName = c.Name;
            }
        }

        return outEntry != 0 && !outName.empty();
    }
}

void RandomPlayerbotMgr::EnsureRandomBotGuilds()
{
    // CMaNGOS parity: Random guild creation is based on random-bot accounts and guild leaders,
    // using a static name pool in `ai_playerbot_guild_names` (characters DB).

    time_t const now = time(nullptr);
    uint32 const minInterval = _randomBotGuildsInitialized ? 300u : 30u;
    if (_randomBotGuildsLastUpdate && (now - _randomBotGuildsLastUpdate) < time_t(minInterval))
        return;
    _randomBotGuildsLastUpdate = now;

    std::vector<uint32> randomBots;
    std::map<uint32, std::vector<uint32>> charAccGuids;

    if (QueryResult charAccounts = CharacterDatabase.PQuery("SELECT `account`, `guid` FROM `characters`"))
    {
        do
        {
            Field* fields = charAccounts->Fetch();
            uint32 accId = fields[0].GetUInt32();
            uint32 guid = fields[1].GetUInt32();
            charAccGuids[accId].push_back(guid);
        } while (charAccounts->NextRow());
    }

    if (!charAccGuids.empty())
    {
        for (uint32 accId : sPlayerbotAIConfig->randomBotAccounts)
            if (!charAccGuids[accId].empty())
                randomBots.insert(randomBots.end(), charAccGuids[accId].begin(), charAccGuids[accId].end());
    }

    if (randomBots.empty())
    {
        _randomBotGuildsInitialized = true;
        return;
    }

    if (sPlayerbotAIConfig->deleteRandomBotGuilds)
    {
        TC_LOG_WARN("playerbots", "Deleting random bot guilds...");
        uint32 counter = 0;
        for (uint32 guidLow : randomBots)
        {
            ObjectGuid leader = ObjectGuid::Create<HighGuid::Player>(guidLow);
            if (Guild* guild = sGuildMgr->GetGuildByLeader(leader))
            {
                guild->Disband();
                ++counter;
            }
        }
        TC_LOG_WARN("playerbots", "%u random bot guild(s) deleted", counter);
        sPlayerbotAIConfig->deleteRandomBotGuilds = false; // one-shot
    }

    if (!sPlayerbotAIConfig->randomBotGuildCount)
    {
        _randomBotGuildsInitialized = true;
        return;
    }

    uint32 guildNumber = 0;
    std::vector<ObjectGuid> availableLeaders;
    for (uint32 guidLow : randomBots)
    {
        ObjectGuid leader = ObjectGuid::Create<HighGuid::Player>(guidLow);
        if (Guild* guild = sGuildMgr->GetGuildByLeader(leader))
        {
            if (std::find(sPlayerbotAIConfig->randomBotGuilds.begin(), sPlayerbotAIConfig->randomBotGuilds.end(), guild->GetId()) == sPlayerbotAIConfig->randomBotGuilds.end())
            {
                ++guildNumber;
                sPlayerbotAIConfig->randomBotGuilds.push_back(guild->GetId());
            }
        }
        else
        {
            Player* player = ObjectAccessor::FindPlayer(leader);
            if (player && !player->GetGuildId() && player->GetLevel() >= 10)
                availableLeaders.push_back(leader);
        }
    }

    if (availableLeaders.empty())
    {
        // Try again later when more random bots are online.
        return;
    }

    uint32 attempts = 0;
    uint32 maxNewGuilds = (sPlayerbotAIConfig->randomBotGuildCount > sPlayerbotAIConfig->randomBotGuilds.size())
        ? (sPlayerbotAIConfig->randomBotGuildCount - uint32(sPlayerbotAIConfig->randomBotGuilds.size()))
        : 0u;
    bool newGuilds = false;

    for (; guildNumber < maxNewGuilds; ++guildNumber)
    {
        ++attempts;
        if (attempts > std::min<uint32>(5, sPlayerbotAIConfig->randomBotGuildCount))
            break;
        if (sPlayerbotAIConfig->randomBotGuilds.size() >= sPlayerbotAIConfig->randomBotGuildCount)
            break;

        std::string guildName = RandomPlayerbotFactory::CreateRandomGuildName();
        if (guildName.empty())
            continue;

        ObjectGuid leader = availableLeaders[urand(0, uint32(availableLeaders.size() - 1))];
        Player* player = ObjectAccessor::FindPlayer(leader);
        if (!player || player->GetGuildId())
            continue;

        Guild* guild = new Guild();
        if (!guild->Create(player, guildName))
        {
            TC_LOG_ERROR("playerbots", "Error creating random guild '%s'", guildName.c_str());
            delete guild;
            continue;
        }

        sGuildMgr->AddGuild(guild);
        sPlayerbotAIConfig->randomBotGuilds.push_back(guild->GetId());
        TC_LOG_INFO("playerbots", "Random Guild <%s>, GM: %s", guildName.c_str(), player->GetName().c_str());
        newGuilds = true;
    }

    if (newGuilds)
        TC_LOG_INFO("playerbots", "Total Random Guilds: %u", uint32(sPlayerbotAIConfig->randomBotGuilds.size()));

    _randomBotGuildsInitialized = true;
}

void RandomPlayerbotMgr::ScaleBotActivity()
{
    float activityPercentage = getActivityPercentage();
    float currentDiff = static_cast<float>(sWorld->GetUpdateTime());

    if (_avgUpdateTime <= 0.0f)
        _avgUpdateTime = currentDiff;
    else
        _avgUpdateTime = _avgUpdateTime * 0.90f + currentDiff * 0.10f;

    float const activityPercentageMod =
        static_cast<float>(pid.calculate(
            HasPlayers() ? sPlayerbotAIConfig->diffWithPlayer : sPlayerbotAIConfig->diffEmpty,
            std::max(_avgUpdateTime, 1.0f)));

    activityPercentage = activityPercentageMod + 50.0f;
    activityPercentage = std::max(0.0f, std::min(100.0f, activityPercentage));
    setActivityPercentage(activityPercentage);

    if (sPlayerbotAIConfig->hasLog("activity_pid.csv"))
    {
        std::ostringstream out;
        out << getMSTime() << ",";
        out << currentDiff << ",";
        out << _avgUpdateTime << ",";
        out << getActivityPercentage() << ",";
        out << activityPercentageMod << ",";
        out << activeBots << ",";
        out << GetPlayerbotsCount() << ",";

        float totalLevel = 0.0f;
        float totalGold = 0.0f;
        float totalGearscore = 0.0f;

        PlayerBotMap bots = GetPlayerBotsSnapshot();
        for (PlayerBotMap::const_iterator itr = bots.begin(); itr != bots.end(); ++itr)
        {
            Player* const bot = itr->second;
            if (!bot)
                continue;

            PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
            if (!botAI || !botAI->AllowActivity(ALL_ACTIVITY))
                continue;

            std::string const bracket = "level:" + std::to_string(bot->GetLevel() / 10);
            float const level = static_cast<float>(bot->GetLevel());
            float const gold = static_cast<float>(bot->GetMoney()) / 10000.0f;
            float const gearscore = static_cast<float>(PlayerbotAI::GetMixedGearScore(bot, false, false));

            totalLevel += level;
            totalGold += gold;
            totalGearscore += gearscore;

            uint32 const botGuid = bot->GetGUID().GetCounter();
            PushMetric(_botPerformanceMetrics[bracket], botGuid, level);
            PushMetric(_botPerformanceMetrics["gold"], botGuid, gold);
            PushMetric(_botPerformanceMetrics["gearscore"], botGuid, gearscore);
        }

        out << std::fixed << std::setprecision(4);
        out << totalLevel << ",";
        for (uint8 i = 0; i < (DEFAULT_MAX_LEVEL / 10) + 1; ++i)
            out << GetMetricDelta(_botPerformanceMetrics["level:" + std::to_string(i)]) * 12 * 60 << ",";
        out << totalGold << ",";
        out << GetMetricDelta(_botPerformanceMetrics["gold"]) * 12 * 60 << ",";
        out << totalGearscore << ",";
        out << GetMetricDelta(_botPerformanceMetrics["gearscore"]) * 12 * 60;

        sPlayerbotAIConfig->log("activity_pid.csv", out.str().c_str());
    }
}

void RandomPlayerbotMgr::QueueDatabasePingQuery()
{
    uint32 const pingStart = getMSTime();
    _queryProcessor.AddCallback(CharacterDatabase.AsyncQuery("SELECT 1").WithCallback(
        [pingStart](QueryResult result)
        {
            DatabasePing(std::move(result), pingStart, "CharacterDatabase");
        }));
}

void RandomPlayerbotMgr::DatabasePing(QueryResult /*result*/, uint32 pingStart, std::string const& db)
{
    sRandomPlayerbotMgr->SetDatabaseDelay(db, GetMSTimeDiffToNow(pingStart));
}

void RandomPlayerbotMgr::LoadNamedLocations()
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    _namedLocations.clear();

    // Keep parity with CMaNGOS table naming if the table exists.
    if (!WorldDatabase.PQuery("SHOW TABLES LIKE 'ai_playerbot_named_location'"))
        return;

    QueryResult result = WorldDatabase.Query(
        "SELECT `name`, `map_id`, `position_x`, `position_y`, `position_z`, `orientation` "
        "FROM `ai_playerbot_named_location` WHERE `name` NOT LIKE 'FISH_LOCATION%%'");
    if (!result)
        return;

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        std::string name = fields[0].GetString();
        uint32 mapId = fields[1].GetUInt32();
        float positionX = fields[2].GetFloat();
        float positionY = fields[3].GetFloat();
        float positionZ = fields[4].GetFloat();
        float orientation = fields[5].GetFloat();

        if (AddNamedLocation(name, WorldLocation(mapId, positionX, positionY, positionZ, orientation)))
            ++count;
    } while (result->NextRow());

    TC_LOG_INFO("playerbots", "Loaded %u named locations", count);
}

bool RandomPlayerbotMgr::AddNamedLocation(std::string const& name, WorldLocation const& location)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    auto [it, inserted] = _namedLocations.emplace(name, location);
    if (!inserted)
    {
        TC_LOG_ERROR("playerbots", "RandomPlayerbotMgr::AddNamedLocation duplicate name '%s'", name.c_str());
        return false;
    }

    return true;
}

bool RandomPlayerbotMgr::GetNamedLocation(std::string const& name, WorldLocation& location)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    auto const itr = _namedLocations.find(name);
    if (itr == _namedLocations.end())
        return false;

    location = itr->second;
    return true;
}

void RandomPlayerbotMgr::SetDatabaseDelay(std::string const& db, uint32 delay)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    _databaseDelay[db] = delay;
}

uint32 RandomPlayerbotMgr::GetDatabaseDelay(std::string const& db) const
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    auto itr = _databaseDelay.find(db);
    if (itr == _databaseDelay.end())
        return 0;

    return itr->second;
}

uint32 RandomPlayerbotMgr::AddRandomBots()
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    uint32 maxAllowedBotCount = GetEventValue(0, "bot_count");

    if (_currentBots.size() < maxAllowedBotCount)
    {
        RefreshRandomBotGuidCache();

        std::unordered_set<uint32> blockedBots;
        if (QueryResult eventResult = CharacterDatabase.PQuery(
                "SELECT bot FROM playerbots_random_bots "
                "WHERE owner = 0 AND `value` <> 0 AND `event` IN ('add', 'logout') "
                "AND (validIn = 0 OR (UNIX_TIMESTAMP() - `time`) < validIn)"))
        {
            do
            {
                Field* eventFields = eventResult->Fetch();
                blockedBots.insert(eventFields[0].GetUInt32());
            } while (eventResult->NextRow());
        }

        maxAllowedBotCount -= _currentBots.size();
        maxAllowedBotCount = std::min(sPlayerbotAIConfig->randomBotsPerInterval, maxAllowedBotCount);
        size_t const cacheSize = _randomBotGuidCache.size();
        size_t const start = cacheSize ? (_randomBotGuidCursor % cacheSize) : 0;
        size_t scanned = 0;
        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        bool hasDbChanges = false;
        uint64 const eventTime = static_cast<uint64>(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());

        for (; scanned < cacheSize && maxAllowedBotCount; ++scanned)
        {
            uint32 guid = _randomBotGuidCache[(start + scanned) % cacheSize];

            if (blockedBots.find(guid) != blockedBots.end())
                continue;
            if (GetPlayerBot(guid))
                continue;
            if (_currentBotSet.find(guid) != _currentBotSet.end())
                continue;

            uint32 add_time = 31104000;

            trans->PAppend(
                "INSERT INTO playerbots_random_bots (owner, bot, `time`, validIn, `event`, `value`, `data`) VALUES (0, %u, " UI64FMTD ", %u, 'add', 1, '') "
                "ON DUPLICATE KEY UPDATE `time` = VALUES(`time`), validIn = VALUES(validIn), `value` = VALUES(`value`), `data` = VALUES(`data`)",
                guid, eventTime, add_time);
            trans->PAppend("DELETE FROM playerbots_random_bots WHERE owner = 0 AND bot = %u AND `event` = 'logout'", guid);
            hasDbChanges = true;

            _currentBots.push_back(guid);
            _currentBotSet.insert(guid);

            auto cacheItr = _eventCache.find(guid);
            if (cacheItr != _eventCache.end() && cacheItr->second.loaded)
            {
                CachedEvent& addEvent = cacheItr->second.events["add"];
                addEvent.value = 1;
                addEvent.lastChangeTime = static_cast<uint32>(time(nullptr));
                addEvent.validIn = add_time;
                addEvent.data.clear();
                cacheItr->second.events.erase("logout");
            }

            maxAllowedBotCount--;
        }

        if (hasDbChanges)
            CharacterDatabase.CommitTransaction(trans);

        if (cacheSize)
            _randomBotGuidCursor = (start + scanned) % cacheSize;

        if (maxAllowedBotCount)
            TC_LOG_ERROR("playerbots", "Not enough random bot accounts available. Try to increase RandomBotAccountCount in your conf file");
    }

    return _currentBots.size();
}

void RandomPlayerbotMgr::RefreshRandomBotGuidCache()
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    uint32 const nowMs = getMSTime();
    constexpr uint32 randomBotGuidCacheRefreshMs = 60 * IN_MILLISECONDS;

    if (_randomBotGuidCacheTimer && GetMSTimeDiffToNow(_randomBotGuidCacheTimer) < randomBotGuidCacheRefreshMs)
        return;

    _randomBotGuidCacheTimer = nowMs;
    _randomBotGuidCache.clear();
    _randomBotGuidCursor = 0;

    if (sPlayerbotAIConfig->randomBotAccounts.empty())
        return;

    std::ostringstream accountList;
    for (size_t i = 0; i < sPlayerbotAIConfig->randomBotAccounts.size(); ++i)
    {
        if (i)
            accountList << ',';
        accountList << sPlayerbotAIConfig->randomBotAccounts[i];
    }

    std::string const query = "SELECT guid FROM characters WHERE online = 0 AND account IN (" + accountList.str() + ")";
    if (QueryResult result = CharacterDatabase.Query(query.c_str()))
    {
        _randomBotGuidCache.reserve(result->GetRowCount());
        do
        {
            Field* fields = result->Fetch();
            _randomBotGuidCache.push_back(fields[0].GetUInt32());
        } while (result->NextRow());

        static thread_local std::mt19937 rnd(std::random_device{}());
        std::shuffle(_randomBotGuidCache.begin(), _randomBotGuidCache.end(), rnd);
    }
}

void RandomPlayerbotMgr::Remove(Player* bot)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    ObjectGuid owner = bot->GetGUID();

    CharacterDatabase.PExecute("DELETE FROM playerbots_random_bots WHERE owner = 0 AND bot = %u", owner.GetCounter());

    BotEventCache& cache = _eventCache[owner.GetCounter()];
    cache.events.clear();
    cache.loaded = false;
    _currentBots.erase(std::remove(_currentBots.begin(), _currentBots.end(), owner.GetCounter()), _currentBots.end());
    _currentBotSet.erase(owner.GetCounter());

    LogoutPlayerBot(owner);
}

void RandomPlayerbotMgr::CheckPlayers()
{
    time_t const now = time(nullptr);
    if (_playersCheckTimer && now < (_playersCheckTimer + 60))
        return;
    _playersCheckTimer = now;

    uint32 newPlayersLevel = 0;
    for (Player* player : GetPlayers())
    {
        if (!player || !player->IsInWorld() || player->IsGameMaster())
            continue;

        newPlayersLevel = std::max<uint32>(newPlayersLevel, player->GetLevel());
    }

    if (!newPlayersLevel)
        newPlayersLevel = sPlayerbotAIConfig->syncLevelNoPlayer;

    _playersLevel = newPlayersLevel;
}

void RandomPlayerbotMgr::CheckBgQueue()
{
    time_t now = time(nullptr);
    if (_bgCheckTimer && now < (_bgCheckTimer + 30))
        return;
    _bgCheckTimer = now;

    for (int bracket = BG_BRACKET_ID_FIRST; bracket < MAX_BATTLEGROUND_BRACKETS; ++bracket)
    {
        for (int queueType = BATTLEGROUND_QUEUE_AV; queueType < MAX_BATTLEGROUND_QUEUE_TYPES; ++queueType)
        {
            BattlegroundData[queueType][bracket] = BattlegroundInfo();
        }
    }

    for (Player* player : GetPlayers())
    {
        if (!player || !player->IsInWorld() || !player->InBattlegroundQueue())
            continue;

        Battleground* bg = player->GetBattleground();
        if (bg && bg->GetStatus() == STATUS_WAIT_LEAVE)
            continue;

        TeamId teamId = player->GetTeamId();

        for (uint8 queueSlot = 0; queueSlot < PLAYER_MAX_BATTLEGROUND_QUEUES; ++queueSlot)
        {
            BattlegroundQueueTypeId queueTypeId = player->GetBattlegroundQueueTypeId(queueSlot);
            if (queueTypeId == BATTLEGROUND_QUEUE_NONE)
                continue;

            BattlegroundTypeId bgTypeId = sBattlegroundMgr->BGTemplateId(queueTypeId);
            Battleground* bgTemplate = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
            if (!bgTemplate)
                continue;

            PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(bgTemplate->GetMapId(), player->GetLevel());
            if (!pvpDiff)
                continue;

            BattlegroundBracketId bracketId = pvpDiff->GetBracketId();
            BattlegroundInfo& info = BattlegroundData[queueTypeId][bracketId];
            info.minLevel = pvpDiff->minLevel;
            info.maxLevel = pvpDiff->maxLevel;

            bool isArena = false;
            bool isRatedArena = false;
            if (uint8 arenaType = BattlegroundMgr::BGArenaType(queueTypeId))
            {
                isArena = true;
                BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);
                GroupQueueInfo ginfo;
                if (bgQueue.GetPlayerGroupInfoData(player->GetGUID(), &ginfo))
                    isRatedArena = ginfo.IsRated;

                if (player->InArena() && player->GetBattleground() && player->GetBattleground()->IsRated())
                    isRatedArena = true;

                if (isRatedArena)
                    info.ratedArenaPlayerCount++;
                else
                    info.skirmishArenaPlayerCount++;
            }
            else
            {
                if (teamId == TEAM_ALLIANCE)
                    info.bgAlliancePlayerCount++;
                else
                    info.bgHordePlayerCount++;
            }

            if (player->InBattleground())
            {
                uint32 instanceId = player->GetBattleground()->GetInstanceID();
                if (player->InArena() && player->GetBattleground()->IsRated())
                {
                    if (std::find(info.ratedArenaInstances.begin(), info.ratedArenaInstances.end(), instanceId) == info.ratedArenaInstances.end())
                        info.ratedArenaInstances.push_back(instanceId);
                    info.ratedArenaInstanceCount = info.ratedArenaInstances.size();
                }
                else if (player->InArena())
                {
                    if (std::find(info.skirmishArenaInstances.begin(), info.skirmishArenaInstances.end(), instanceId) == info.skirmishArenaInstances.end())
                        info.skirmishArenaInstances.push_back(instanceId);
                    info.skirmishArenaInstanceCount = info.skirmishArenaInstances.size();
                }
                else
                {
                    if (std::find(info.bgInstances.begin(), info.bgInstances.end(), instanceId) == info.bgInstances.end())
                        info.bgInstances.push_back(instanceId);
                    info.bgInstanceCount = info.bgInstances.size();
                }
            }

            if (!player->IsInvitedForBattlegroundQueueType(queueTypeId) && !player->InBattleground())
            {
                if (isRatedArena)
                    info.activeRatedArenaQueue = 1;
                else if (isArena)
                    info.activeSkirmishArenaQueue = 1;
                else
                    info.activeBgQueue = 1;
            }
        }
    }

    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = bots.begin(); it != bots.end(); ++it)
    {
        Player* bot = it->second;
        if (!bot || !bot->IsInWorld() || !IsRandomBot(bot))
            continue;

        if (!bot->InBattlegroundQueue() && !bot->InBattleground())
            continue;

        Battleground* bg = bot->GetBattleground();
        if (bg && bg->GetStatus() == STATUS_WAIT_LEAVE)
            continue;

        TeamId teamId = bot->GetTeamId();

        for (uint8 queueSlot = 0; queueSlot < PLAYER_MAX_BATTLEGROUND_QUEUES; ++queueSlot)
        {
            BattlegroundQueueTypeId queueTypeId = bot->GetBattlegroundQueueTypeId(queueSlot);
            if (queueTypeId == BATTLEGROUND_QUEUE_NONE)
                continue;

            BattlegroundTypeId bgTypeId = sBattlegroundMgr->BGTemplateId(queueTypeId);
            Battleground* bgTemplate = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
            if (!bgTemplate)
                continue;

            PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(bgTemplate->GetMapId(), bot->GetLevel());
            if (!pvpDiff)
                continue;

            BattlegroundBracketId bracketId = pvpDiff->GetBracketId();
            BattlegroundInfo& info = BattlegroundData[queueTypeId][bracketId];
            info.minLevel = pvpDiff->minLevel;
            info.maxLevel = pvpDiff->maxLevel;

            bool isArena = false;
            bool isRatedArena = false;
            if (uint8 arenaType = BattlegroundMgr::BGArenaType(queueTypeId))
            {
                isArena = true;
                BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(queueTypeId);
                GroupQueueInfo ginfo;
                if (bgQueue.GetPlayerGroupInfoData(bot->GetGUID(), &ginfo))
                    isRatedArena = ginfo.IsRated;

                if (bot->InArena() && bot->GetBattleground() && bot->GetBattleground()->IsRated())
                    isRatedArena = true;

                if (isRatedArena)
                    info.ratedArenaBotCount++;
                else
                    info.skirmishArenaBotCount++;
            }
            else
            {
                if (teamId == TEAM_ALLIANCE)
                    info.bgAllianceBotCount++;
                else
                    info.bgHordeBotCount++;
            }

            if (bot->InBattleground() && bot->GetBattleground())
            {
                uint32 instanceId = bot->GetBattleground()->GetInstanceID();
                if (bot->InArena() && bot->GetBattleground()->IsRated())
                {
                    if (std::find(info.ratedArenaInstances.begin(), info.ratedArenaInstances.end(), instanceId) == info.ratedArenaInstances.end())
                        info.ratedArenaInstances.push_back(instanceId);
                    info.ratedArenaInstanceCount = info.ratedArenaInstances.size();
                }
                else if (bot->InArena())
                {
                    if (std::find(info.skirmishArenaInstances.begin(), info.skirmishArenaInstances.end(), instanceId) == info.skirmishArenaInstances.end())
                        info.skirmishArenaInstances.push_back(instanceId);
                    info.skirmishArenaInstanceCount = info.skirmishArenaInstances.size();
                }
                else
                {
                    if (std::find(info.bgInstances.begin(), info.bgInstances.end(), instanceId) == info.bgInstances.end())
                        info.bgInstances.push_back(instanceId);
                    info.bgInstanceCount = info.bgInstances.size();
                }
            }
        }
    }

    if (sPlayerbotAIConfig->randomBotAutoJoinBG && GetPlayerbotsCount() >= GetMaxAllowedBotCount())
    {
        auto parseBrackets = [](std::string const& data)
        {
            std::vector<uint32> result;
            std::vector<std::string> tokens = split(data, ',');
            for (std::string const& token : tokens)
            {
                if (token.empty())
                    continue;
                result.push_back(static_cast<uint32>(atoi(token.c_str())));
            }
            return result;
        };

        auto updateBgInstanceCount = [&](BattlegroundQueueTypeId queueTypeId, std::vector<uint32> const& brackets, uint32 minCount)
        {
            for (uint32 bracket : brackets)
            {
                BattlegroundInfo& info = BattlegroundData[queueTypeId][bracket];
                if (!info.activeBgQueue && info.bgInstanceCount < minCount && info.bgInstances.size() < minCount)
                    info.activeBgQueue = 1;
            }
        };

        auto updateRatedArenaInstanceCount = [&](BattlegroundQueueTypeId queueTypeId, uint32 bracket, uint32 minCount)
        {
            BattlegroundInfo& info = BattlegroundData[queueTypeId][bracket];
            if (!info.activeRatedArenaQueue && info.ratedArenaInstanceCount < minCount && info.ratedArenaInstances.size() < minCount)
                info.activeRatedArenaQueue = 1;
        };

        updateRatedArenaInstanceCount(BATTLEGROUND_QUEUE_2v2, sPlayerbotAIConfig->randomBotAutoJoinArenaBracket,
            sPlayerbotAIConfig->randomBotAutoJoinBGRatedArena2v2Count);
        updateRatedArenaInstanceCount(BATTLEGROUND_QUEUE_3v3, sPlayerbotAIConfig->randomBotAutoJoinArenaBracket,
            sPlayerbotAIConfig->randomBotAutoJoinBGRatedArena3v3Count);
        updateRatedArenaInstanceCount(BATTLEGROUND_QUEUE_5v5, sPlayerbotAIConfig->randomBotAutoJoinArenaBracket,
            sPlayerbotAIConfig->randomBotAutoJoinBGRatedArena5v5Count);

        updateBgInstanceCount(BATTLEGROUND_QUEUE_IC, parseBrackets(sPlayerbotAIConfig->randomBotAutoJoinICBrackets),
            sPlayerbotAIConfig->randomBotAutoJoinBGICCount);
        updateBgInstanceCount(BATTLEGROUND_QUEUE_EY, parseBrackets(sPlayerbotAIConfig->randomBotAutoJoinEYBrackets),
            sPlayerbotAIConfig->randomBotAutoJoinBGEYCount);
        updateBgInstanceCount(BATTLEGROUND_QUEUE_AV, parseBrackets(sPlayerbotAIConfig->randomBotAutoJoinAVBrackets),
            sPlayerbotAIConfig->randomBotAutoJoinBGAVCount);
        updateBgInstanceCount(BATTLEGROUND_QUEUE_AB, parseBrackets(sPlayerbotAIConfig->randomBotAutoJoinABBrackets),
            sPlayerbotAIConfig->randomBotAutoJoinBGABCount);
        updateBgInstanceCount(BATTLEGROUND_QUEUE_WS, parseBrackets(sPlayerbotAIConfig->randomBotAutoJoinWSBrackets),
            sPlayerbotAIConfig->randomBotAutoJoinBGWSCount);
    }
}

void RandomPlayerbotMgr::CheckLfgQueue()
{
    time_t now = time(nullptr);
    if (_lfgCheckTimer && now < (_lfgCheckTimer + 30))
        return;
    _lfgCheckTimer = now;

    TC_LOG_DEBUG("playerbots", "Checking LFG Queue...");

    LfgDungeons[TEAM_ALLIANCE].clear();
    LfgDungeons[TEAM_HORDE].clear();

    for (Player* player : GetPlayers())
    {
        if (!player || !player->IsInWorld())
            continue;

        Group* group = player->GetGroup();
        ObjectGuid guid = group ? group->GetGUID() : player->GetGUID();

        uint32 queueId = sLFGMgr->GetActiveQueueId(guid);
        if (!queueId)
            continue;

        lfg::LfgState state = sLFGMgr->GetState(guid, queueId);
        if (state != lfg::LFG_STATE_NONE && state < lfg::LFG_STATE_DUNGEON)
        {
            uint32 playerQueueId = sLFGMgr->GetActiveQueueId(player->GetGUID());
            if (!playerQueueId)
                continue;

            lfg::LfgDungeonSet const& dungeons = sLFGMgr->GetSelectedDungeons(player->GetGUID(), playerQueueId);
            for (uint32 dungeonId : dungeons)
            {
                lfg::LFGDungeonData const* dungeon = sLFGMgr->GetLFGDungeon(dungeonId);
                if (!dungeon)
                    continue;

                LfgDungeons[player->GetTeamId()].push_back(dungeon->id);
            }
        }
    }

    TC_LOG_DEBUG("playerbots", "LFG Queue check finished");
}

void RandomPlayerbotMgr::AddOfflineGroupBots()
{
    time_t const now = time(nullptr);
    if (_offlineGroupBotsTimer && now < (_offlineGroupBotsTimer + 5))
        return;
    _offlineGroupBotsTimer = now;

    uint32 const maxLoginsPerInterval = std::max<uint32>(1, sPlayerbotAIConfig->randomBotsMaxLoginsPerInterval);
    uint32 pendingLogins = static_cast<uint32>(GetBotLoadingCount());
    if (pendingLogins >= maxLoginsPerInterval)
        return;

    uint32 loginBudget = maxLoginsPerInterval - pendingLogins;
    uint32 totalCounter = 0;

    for (Player* player : GetPlayers())
    {
        if (!player || !player->IsInWorld())
            continue;

        Group* group = player->GetGroup();
        if (!group || !group->IsLeader(player->GetGUID()))
            continue;

        Group::MemberSlotList const& slots = group->GetMemberSlots();
        for (Group::MemberSlotList::const_iterator itr = slots.begin(); itr != slots.end(); ++itr)
        {
            ObjectGuid member = itr->guid;
            if (member == player->GetGUID())
                continue;

            if (!loginBudget)
                break;

            if (ObjectAccessor::FindConnectedPlayer(member))
                continue;

            if (GetPlayerBot(member))
                continue;

            if (IsBotLoading(member))
                continue;

            uint32 account = sCharacterCache->GetCharacterAccountIdByGuid(member);
            if (!sPlayerbotAIConfig->IsInRandomAccountList(account))
                continue;

            if (AddRandomBot(member.GetCounter()))
            {
                ++totalCounter;
                --loginBudget;
            }
        }

        if (!loginBudget)
            break;
    }

    if (totalCounter)
        TC_LOG_INFO("playerbots", "Added %u offline group bots", totalCounter);
}

void RandomPlayerbotMgr::LoginFreeBots()
{
    AddOfflineGroupBots();
}

Item* RandomPlayerbotMgr::CreateTempItem(uint32 item, uint32 count, Player const* player, uint32 randomPropertyId)
{
    if (count < 1)
        return nullptr;

    Item* pItem = Item::CreateItem(item, count, player, true);
    if (!pItem)
        return nullptr;

    if (randomPropertyId)
        pItem->SetItemRandomProperties(int32(randomPropertyId));

    return pItem;
}

InventoryResult RandomPlayerbotMgr::CanEquipUnseenItem(Player* player, uint8 slot, uint16& dest, uint32 item)
{
    dest = 0;

    Item* pItem = RandomPlayerbotMgr::CreateTempItem(item, 1, player);
    if (!pItem)
        return EQUIP_ERR_ITEM_NOT_FOUND;

    PlayerbotAI* botAI = GET_PLAYERBOT_AI(player);
    InventoryResult result = botAI ? botAI->CanEquipItem(slot, dest, pItem, true, true) :
                                     player->CanEquipItem(slot, dest, pItem, true, true);
    pItem->RemoveFromUpdateQueueOf(player);
    delete pItem;

    return result;
}

bool RandomPlayerbotMgr::ProcessBot(uint32 bot)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    Player* player = GetPlayerBot(bot);
    PlayerbotAI* botAI = player ? GET_PLAYERBOT_AI(player) : nullptr;
    ObjectGuid botGUID = ObjectGuid::Create<HighGuid::Player>(bot);
    bool const botsAllowedInWorld = !sPlayerbotAIConfig->randomBotLoginWithPlayer || HasPlayers();

    uint32 isValid = GetEventValue(bot, "add");
    if (!botsAllowedInWorld)
        isValid = 0;

    if (!isValid)
    {
        if (botsAllowedInWorld && player && player->GetGroup())
        {
            SetEventValue(bot, "add", 1, 120);
            return false;
        }

        if (player)
        {
            auto side = (player->GetTeamId() == TeamId::TEAM_ALLIANCE ? "A" : "H");
            TC_LOG_INFO("playerbots", "Bot #%u %s:%u <%s>: log out", bot, side, player->GetLevel(), player->GetName().c_str());
        }
        else
            TC_LOG_INFO("playerbots", "Bot #%u: log out", bot);

        SetEventValue(bot, "add", 0, 0);
        _currentBots.erase(std::remove(_currentBots.begin(), _currentBots.end(), bot), _currentBots.end());
        _currentBotSet.erase(bot);

        if (player)
            LogoutPlayerBot(botGUID);

        return false;
    }

    uint32 randomTime;
    if (!player)
    {
        if (!botsAllowedInWorld)
            return false;

        if (IsBotLoading(botGUID))
            return false;

        AddPlayerBot(botGUID, 0);
        if (!IsBotLoading(botGUID))
            return false;

        randomTime = 1 + (std::rand() % 2);

        uint32 randomBotUpdateInterval = _isBotInitializing ? 1 : sPlayerbotAIConfig->randomBotUpdateInterval;
        int minValue = std::max(5, static_cast<int>(randomBotUpdateInterval * 0.5));
        int maxValue = std::max(12, static_cast<int>(randomBotUpdateInterval * 2));
        randomTime = minValue + (std::rand() % (maxValue - minValue + 1));
        
        SetEventValue(bot, "update", 1, randomTime);

        // do not randomize or teleport immediately after server start (prevent lagging)
        if (!GetEventValue(bot, "randomize"))
        {
            int minValue = std::max(7, static_cast<int>(randomBotUpdateInterval * 0.7));
            int maxValue = std::max(14, static_cast<int>(randomBotUpdateInterval * 1.4));
            randomTime = minValue + (std::rand() % (maxValue - minValue + 1));

            ScheduleRandomize(bot, randomTime);
        }
        if (!GetEventValue(bot, "teleport"))
        {
            int minValue = std::max(7, static_cast<int>(randomBotUpdateInterval * 0.7));
            int maxValue = std::max(14, static_cast<int>(randomBotUpdateInterval * 1.4));
            randomTime = minValue + (std::rand() % (maxValue - minValue + 1));

            ScheduleTeleport(bot, randomTime);
        }
        if (!GetEventValue(bot, "change_strategy"))
        {
            int minValue = std::max(7, static_cast<int>(randomBotUpdateInterval * 0.7));
            int maxValue = std::max(14, static_cast<int>(randomBotUpdateInterval * 1.4));
            randomTime = minValue + (std::rand() % (maxValue - minValue + 1));

            ScheduleChangeStrategy(bot, randomTime);
        }

        return true;
    }

    if (!player->IsInWorld())
        return false;

    if (player->GetGroup() || player->HasUnitState(UNIT_STATE_IN_FLIGHT))
        return false;

    uint32 update = GetEventValue(bot, "update");
    if (!update)
    {
        if (botAI)
            botAI->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(true);

        bool update = true;
        if (botAI)
        {
            botAI->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(true);
            if (!sRandomPlayerbotMgr->IsRandomBot(player))
                update = false;

            if (player->GetGroup() && botAI->GetGroupMaster())
            {
                PlayerbotAI* groupMasterBotAI = GET_PLAYERBOT_AI(botAI->GetGroupMaster());
                if (!groupMasterBotAI || groupMasterBotAI->IsRealPlayer())
                {
                    update = false;
                }
            }

            if (botAI->HasPlayerNearby(sPlayerbotAIConfig->grindDistance))
                update = false;
        }

        if (update && !sPlayerbotAIConfig->disableRandomLevels)
            ProcessBot(player);

        randomTime = urand(sPlayerbotAIConfig->minRandomBotReviveTime, sPlayerbotAIConfig->maxRandomBotReviveTime * 5);
        SetEventValue(bot, "update", 1, randomTime);

        return true;
    }

    /*uint32 logout = GetEventValue(bot, "logout");
    if (player && !logout && !isValid)
    {
        auto side = (player->GetTeamId() == TeamId::TEAM_ALLIANCE ? "A" : "H");
        TC_LOG_INFO("playerbots", "Bot #%u %s:%u <%s>: log out", bot, side, player->GetLevel(), player->GetName().c_str());
        LogoutPlayerBot(botGUID);
        for (auto it = _currentBots.begin(); it != _currentBots.end();)
        {
            if (*it == bot)
            {
                it = _currentBots.erase(it);
                break;
            }
            else
                ++it;
        }
        SetEventValue(bot, "logout", 1, urand(sPlayerbotAIConfig->minRandomBotInWorldTime, sPlayerbotAIConfig->maxRandomBotInWorldTime));
        return true;
    }*/

    return false;
}

bool RandomPlayerbotMgr::ProcessBot(Player* player)
{
    if (!player || !player->IsInWorld() || player->IsBeingTeleported())
        return false;

    if (!player->GetSession() || player->GetSession()->isLogingOut())
        return false;

    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    ObjectGuid bot = player->GetGUID();

    if (player->InBattleground())
        return false;

    if (player->InBattlegroundQueue())
        return false;

    

    // if death revive
    if (player->isDead())
    {
        if (!GetEventValue(bot, "dead"))
        {
            uint32 randomTime = urand(sPlayerbotAIConfig->minRandomBotReviveTime, sPlayerbotAIConfig->maxRandomBotReviveTime);
            TC_LOG_DEBUG("playerbots", "Mark bot %s as dead, will be revived in %us.", player->GetName().c_str(), randomTime);
            SetEventValue(bot, "dead", 1, sPlayerbotAIConfig->maxRandomBotInWorldTime);
            SetEventValue(bot, "revive", 1, randomTime);
            return false;
        }

        if (!GetEventValue(bot, "revive"))
        {
            Revive(player);
            return true;
        }

        return false;
    }

    // only randomize and teleport idle bots
    bool idleBot = false;
    PlayerbotAI* botAI = GET_PLAYERBOT_AI(player);
    if (botAI)
    {
        if (TravelTarget* target = botAI->GetAiObjectContext()->GetValue<TravelTarget*>("travel target")->Get())
            idleBot = target->getTravelState() == TravelState::TRAVEL_STATE_IDLE;
        else
            idleBot = true;
    }
    if (idleBot)
    {
        if (player->GetInstanceId() || player->GetGroup() || player->InArena() || player->inRandomLfgDungeon() || player->IsInFlight() || player->IsInCombat() || !player->IsAlive())
            return false;

        // randomize
        uint32 randomize = GetEventValue(bot, "randomize");
        if (!randomize)
        {
            auto pmo = sPerformanceMonitor->start(PERF_MON_RNDBOT, "Randomize");
            Randomize(player);
            TC_LOG_DEBUG("playerbots", "Bot #%u %s:%u <%s>: randomized", bot.GetCounter(), player->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", player->GetLevel(), player->GetName().c_str());
            if (pmo)
                pmo->finish();
            return true;
        }

        uint32 teleport = GetEventValue(bot, "teleport");
        if (!teleport && HasPlayers())
        {
            auto pmo = sPerformanceMonitor->start(PERF_MON_RNDBOT, "RandomTeleportByLocations");
            TC_LOG_DEBUG("playerbots", "Bot #%u <%s>: teleport for level and refresh", bot.GetCounter(), player->GetName().c_str());
            Refresh(player);
            RandomTeleportForLevel(player);
            uint32 time = urand(sPlayerbotAIConfig->minRandomBotTeleportInterval, sPlayerbotAIConfig->maxRandomBotTeleportInterval);
            ScheduleTeleport(bot, time);
            if (pmo)
                pmo->finish();
            return true;
        }

        uint32 changeStrategy = GetEventValue(bot, "change_strategy");
        if (!changeStrategy)
        {
            ChangeStrategy(player);
            return true;
        }
    }

    return false;
}

void RandomPlayerbotMgr::TagForRandomize(Player* bot, uint32 level)
{
    ObjectGuid::LowType guid = bot->GetGUID().GetCounter();

    SetValue(bot, "level", level);
    ScheduleRandomize(guid, 0);
}

void RandomPlayerbotMgr::Revive(Player* player)
{
    uint64 bot = player->GetGUID();

    SetEventValue(bot, "dead", 0, 0);
    SetEventValue(bot, "revive", 0, 0);

    Refresh(player);
}

void RandomPlayerbotMgr::ChangeStrategy(Player* player)
{
    if (!player)
        return;

    PlayerbotAI* botAI = GET_PLAYERBOT_AI(player);
    if (!botAI)
        return;

    botAI->ResetStrategies();
    ScheduleChangeStrategy(player->GetGUID().GetCounter());
}

void RandomPlayerbotMgr::Clear(Player* bot)
{
    //PlayerbotFactory factory(bot, bot->GetLevel());
    //factory.ClearEverything();
}

void RandomPlayerbotMgr::RandomizeFirst(Player* bot)
{
    uint32 maxLevel = sPlayerbotAIConfig->randomBotMaxLevel;
    maxLevel = std::min(maxLevel, sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));

    // if lvl sync is enabled, max level is limited by online players lvl
    if (sPlayerbotAIConfig->syncLevelWithPlayers)
    {
        uint32 syncedMax = std::min(_playersLevel + sPlayerbotAIConfig->syncLevelMaxAbove,
            uint32(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)));
        maxLevel = std::max(sPlayerbotAIConfig->randomBotMinLevel, syncedMax);
    }

    //PerformanceMonitorOperation* pmo = sPerformanceMonitor->start(PERF_MON_RNDBOT, "RandomizeFirst");

    
    uint32 level = urand(sPlayerbotAIConfig->randomBotMinLevel, maxLevel);
    uint32 chance = std::clamp<uint32>(sPlayerbotAIConfig->randomBotMaxLevelChance * 100, 0, 100);
    if (urand(1, 100) <= chance)
        level = maxLevel;

    if (bot->GetClass() == CLASS_DEATH_KNIGHT)
        level = urand(std::max(sPlayerbotAIConfig->randomBotMinLevel, sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)), std::max(sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL), maxLevel));

    if (sPlayerbotAIConfig->disableRandomLevels)
    {
        uint32 startLevel = std::clamp(sPlayerbotAIConfig->randombotStartingLevel, sPlayerbotAIConfig->randomBotMinLevel, maxLevel);
        uint32 dkStartLevel = std::max(startLevel, sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL));
        level = bot->GetClass() == CLASS_DEATH_KNIGHT ? dkStartLevel : startLevel;
    }

    SetValue(bot, "level", level);
    BotFactory factory(bot, level);
    factory.Randomize(false);
    factory.InitTalentsTree(true);
    factory.InitEquipment(true);
    if (bot->GetLevel() >= sPlayerbotAIConfig->minEnchantingBotLevel)
        factory.ApplyEnchantAndGemsNew();

    uint32 randomTime   = urand(sPlayerbotAIConfig->minRandomBotRandomizeTime, sPlayerbotAIConfig->maxRandomBotRandomizeTime);
    uint32 inworldTime  = urand(sPlayerbotAIConfig->minRandomBotInWorldTime, sPlayerbotAIConfig->maxRandomBotInWorldTime);

    CharacterDatabase.PExecute("UPDATE playerbots_random_bots SET validIn = %u WHERE `event` = 'bot_delete' AND bot = %u",
        randomTime, bot->GetGUID().GetCounter());
    CharacterDatabase.PExecute("UPDATE playerbots_random_bots SET validIn = %u WHERE `event` = 'logout' AND bot = %u",
        inworldTime, bot->GetGUID().GetCounter());

    // teleport to a random inn for bot level
    if (GET_PLAYERBOT_AI(bot))
        GET_PLAYERBOT_AI(bot)->Reset(true);

    if (bot->GetGroup())
        bot->RemoveFromGroup();

    //if (pmo)
        //pmo->finish();

    //for (int i = 1; i < MAX_INVTYPE; ++i)
    //{
        //sRandomItemMgr->FindBestItemForLevelAndEquip(bot, InventoryType(i));
    //}

    RandomTeleportForLevel(bot);
}

void RandomPlayerbotMgr::Randomize(Player* bot)
{
    if (GET_PLAYERBOT_AI(bot))
            GET_PLAYERBOT_AI(bot)->Reset(true);

    bool initialRandom = false;
    if (bot->GetLevel() <= sPlayerbotAIConfig->randombotStartingLevel)
        initialRandom = true;

    if (!initialRandom && sPlayerbotAIConfig->syncLevelWithPlayers && HasPlayers())
    {
        uint32 syncedMax = std::max(sPlayerbotAIConfig->randomBotMinLevel,
            std::min(_playersLevel + sPlayerbotAIConfig->syncLevelMaxAbove,
                uint32(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))));
        if (bot->GetLevel() > syncedMax || (bot->GetLevel() + sPlayerbotAIConfig->syncLevelMaxAbove) < _playersLevel)
            initialRandom = true;
    }

    uint32 level = GetValue(bot, "level");
    uint32 maxLevel = std::min(sPlayerbotAIConfig->randomBotMaxLevel, sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));
    uint32 minLevel = std::min(sPlayerbotAIConfig->randomBotMinLevel, maxLevel);
    if (initialRandom || !level || level < minLevel || level > maxLevel)
    {
        RandomizeFirst(bot);
        return;
    }
    BotFactory factory(bot, level);
    factory.Randomize(false);
    factory.InitTalentsTree(true);
    factory.InitEquipment(true);
    if (bot->GetLevel() >= sPlayerbotAIConfig->minEnchantingBotLevel)
        factory.ApplyEnchantAndGemsNew();

    // rez / clean cd / etc
    Refresh(bot);
    RandomTeleportForLevel(bot);

    uint32 randomTime = urand(sPlayerbotAIConfig->minRandomBotRandomizeTime, sPlayerbotAIConfig->maxRandomBotRandomizeTime);
    ScheduleRandomize(bot->GetGUID().GetCounter(), randomTime);
}

void RandomPlayerbotMgr::InstaRandomize(Player* bot)
{
    if (!bot)
        return;

    RandomizeFirst(bot);
}

void RandomPlayerbotMgr::UpdateGearSpells(Player* bot)
{
    if (!bot)
        return;

    BotFactory factory(bot, bot->GetLevel());
    factory.InitTalentsTree(true);
    factory.InitEquipment(true);
    if (bot->GetLevel() >= sPlayerbotAIConfig->minEnchantingBotLevel)
        factory.ApplyEnchantAndGemsNew();
}

void RandomPlayerbotMgr::Refresh(Player* bot)
{
    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
    if (!botAI)
        return;

    if (bot->isDead())
    {
        bot->ResurrectPlayer(1.0f);
        bot->SpawnCorpseBones();
        botAI->ResetStrategies();
    }

    // if (sPlayerbotAIConfig->disableRandomLevels)
    //     return;

    if (bot->InBattleground())
        return;

    TC_LOG_INFO("playerbots", "Refreshing bot #%u <%s>", bot->GetGUID().GetCounter(), bot->GetName().c_str());
    auto pmo = sPerformanceMonitor->start(PERF_MON_RNDBOT, "Refresh");

    botAI->Reset();
    bot->DurabilityRepairAll(false, 1.0f, false);
    bot->SetFullHealth();
    //bot->SetPvP(true);
    BotFactory factory(bot, bot->GetLevel());
    factory.Refresh();

    if (bot->GetMaxPower(POWER_MANA) > 0)
        bot->SetPower(POWER_MANA, bot->GetMaxPower(POWER_MANA));

    if (bot->GetMaxPower(POWER_ENERGY) > 0)
        bot->SetPower(POWER_ENERGY, bot->GetMaxPower(POWER_ENERGY));

    uint32 money = bot->GetMoney();
    bot->SetMoney(money + 500 * sqrt(urand(1, bot->GetLevel() * 5)));

    //if (bot->GetGroup())
        //bot->RemoveFromGroup();

    if (pmo)
        pmo->finish();
}

bool RandomPlayerbotMgr::IsRandomBot(Player* bot)
{
    if (bot && GET_PLAYERBOT_AI(bot))
    {
        if (GET_PLAYERBOT_AI(bot)->IsRealPlayer())
        {
            return false;
        }
    }
    if (bot)
    {
        return IsRandomBot(bot->GetGUID().GetCounter());
    }

    return false;
}

bool RandomPlayerbotMgr::IsRandomBot(ObjectGuid::LowType bot)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(bot);
    if (!sPlayerbotAIConfig->IsInRandomAccountList(sCharacterCache->GetCharacterAccountIdByGuid(guid)))
        return false;
    if (_currentBotSet.find(bot) != _currentBotSet.end())
        return true;
    return false;
}

void RandomPlayerbotMgr::GetBots()
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    if (!_currentBots.empty())
        return;

    uint32 maxAllowedBotCount = GetEventValue(0, "bot_count");
    if (QueryResult result = CharacterDatabase.PQuery(
            "SELECT bot FROM playerbots_random_bots "
            "WHERE owner = 0 AND `event` = 'add' AND `value` <> 0 "
            "AND (validIn = 0 OR (UNIX_TIMESTAMP() - `time`) < validIn)"))
    {
        do
        {
            Field* fields = result->Fetch();
            _currentBots.push_back(fields[0].GetUInt32());
            _currentBotSet.insert(fields[0].GetUInt32());

            if (_currentBots.size() >= maxAllowedBotCount)
                break;
        } while (result->NextRow());
    }
}

std::vector<uint32> RandomPlayerbotMgr::GetBgBots(uint32 bracket)
{
    std::vector<uint32> bgBots;
    if (QueryResult result = CharacterDatabase.PQuery(
            "SELECT bot FROM playerbots_random_bots "
            "WHERE owner = 0 AND `event` = 'bg' AND `value` = %u "
            "AND (validIn = 0 OR (UNIX_TIMESTAMP() - `time`) < validIn)",
            bracket))
    {
        do
        {
            Field* fields = result->Fetch();
            bgBots.push_back(fields[0].GetUInt32());
        } while (result->NextRow());
    }

    return bgBots;
}

CachedEvent* RandomPlayerbotMgr::FindEvent(uint32 bot, std::string const& event)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    BotEventCache& cache = _eventCache[bot];

    // Load all events for this bot once.
    if (!cache.loaded)
    {
        cache.events.clear();
        if (QueryResult result = CharacterDatabase.PQuery("SELECT `event`, `value`, `time`, validIn, `data` FROM playerbots_random_bots WHERE owner = 0 AND bot = %u", bot))
        {
            do
            {
                Field* fields = result->Fetch();
                std::string const eventName = fields[0].GetString();

                CachedEvent e;
                e.value = fields[1].GetUInt32();
                e.lastChangeTime = fields[2].GetUInt32();
                e.validIn = fields[3].GetUInt32();
                e.data = fields[4].GetString();
                cache.events.emplace(eventName, std::move(e));
            } while (result->NextRow());
        }

        cache.loaded = true;
    }

    auto eventItr = cache.events.find(event);
    if (eventItr == cache.events.end())
        return nullptr;

    CachedEvent& e = eventItr->second;
    if (e.validIn && (time(nullptr) - e.lastChangeTime) >= e.validIn &&
        event != "specNo" && event != "specLink" && event != "level")
    {
        cache.events.erase(eventItr);
        return nullptr;
    }

    return &e;
}

uint32 RandomPlayerbotMgr::GetEventValue(uint32 bot, std::string const event)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    if (CachedEvent* e = FindEvent(bot, event))
        return e->value;

    return 0;
}

std::string const RandomPlayerbotMgr::GetEventData(uint32 bot, std::string const event)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    if (CachedEvent* e = FindEvent(bot, event))
        return e->data;

    return "";
}

uint32 RandomPlayerbotMgr::SetEventValue(uint32 bot, std::string const event, uint32 value, uint32 validIn,
    std::string const data)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    BotEventCache& cache = _eventCache[bot];
    if (cache.loaded && value == 0)
    {
        auto itr = cache.events.find(event);
        if (itr == cache.events.end() || itr->second.value == 0)
            return 0;
    }

    if (value)
    {
        uint64 eventTime = static_cast<uint64>(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        CharacterDatabase.PExecute(
            "INSERT INTO playerbots_random_bots (owner, bot, `time`, validIn, `event`, `value`, `data`) VALUES (0, %u, " UI64FMTD ", %u, '%s', %u, '%s') "
            "ON DUPLICATE KEY UPDATE `time` = VALUES(`time`), validIn = VALUES(validIn), `value` = VALUES(`value`), `data` = VALUES(`data`)",
            bot, eventTime, validIn, event.c_str(), value, data.c_str());
    }
    else
    {
        CharacterDatabase.PExecute("DELETE FROM playerbots_random_bots WHERE owner = 0 AND bot = %u AND `event` = '%s'",
            bot, event.c_str());
    }

    if (cache.loaded)
    {
        if (value == 0)
        {
            cache.events.erase(event);
        }
        else
        {
            CachedEvent& cachedEvent = cache.events[event];
            cachedEvent.value = value;
            cachedEvent.lastChangeTime = static_cast<uint32>(time(nullptr));
            cachedEvent.validIn = validIn;
            cachedEvent.data = data;
        }
    }

    return value;
}

uint32 RandomPlayerbotMgr::GetValue(uint32 bot, std::string const type) { return GetEventValue(bot, type); }

uint32 RandomPlayerbotMgr::GetValue(Player* bot, std::string const type)
{
    return GetValue(bot->GetGUID().GetCounter(), type);
}

int32 RandomPlayerbotMgr::GetValueValidTime(uint32 bot, std::string const event)
{
    if (CachedEvent* cached = FindEvent(bot, event))
        return static_cast<int32>(cached->validIn) - static_cast<int32>(time(nullptr) - cached->lastChangeTime);

    return 0;
}

std::string const RandomPlayerbotMgr::GetData(uint32 bot, std::string const type) { return GetEventData(bot, type); }

void RandomPlayerbotMgr::SetValue(uint32 bot, std::string const type, uint32 value, std::string const data)
{
    SetEventValue(bot, type, value, sPlayerbotAIConfig->maxRandomBotInWorldTime, data);
}

void RandomPlayerbotMgr::SetValue(Player* bot, std::string const type, uint32 value, std::string const data)
{
    SetValue(bot->GetGUID().GetCounter(), type, value, data);
}

void RandomPlayerbotMgr::OnPlayerLogout(Player* player)
{
    if (!player)
        return;

    DisablePlayerBot(player->GetGUID());

    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = bots.begin(); it != bots.end(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
        if (botAI && player == botAI->GetMaster())
        {
            botAI->SetMaster(nullptr);
            if (!bot->InBattleground())
            {
                botAI->ResetStrategies();
            }
        }
    }

    std::lock_guard<std::mutex> lock(_playersMutex);
    std::vector<Player*>::iterator i = std::find(_players.begin(), _players.end(), player);
    if (i != _players.end())
    {
        _players.erase(i);
        uint32 count = _realPlayersCount.load(std::memory_order_relaxed);
        if (count > 0)
            _realPlayersCount.store(count - 1, std::memory_order_relaxed);
    }
}

void RandomPlayerbotMgr::OnBotLoginInternal(Player* const bot)
{
    auto maxAllowed = sRandomPlayerbotMgr->GetMaxAllowedBotCount();
    TC_LOG_INFO("playerbots", "%u/%u Bot %s logged in - Active spec tab: %u Spec: %u", GetPlayerbotsCount(), maxAllowed, bot->GetName().c_str(), (uint32)bot->GetActiveSpec(), (uint32)bot->GetSpecialization());

    // If this player has been created recently and is not assign horde / alliance as pandaren
    if (bot->GetRace() == RACE_PANDAREN_NEUTRAL)
    {
        // Allow player to get fully logged
        bot->AddDelayedEvent(5000, [bot]()
        {
            static const uint32 JOIN_THE_ALLIANCE = 1;
            static const uint32 JOIN_THE_HORDE = 0;

            WorldPacket packet(CMSG_SELECT_FACTION);
            packet << (std::rand() % 2 ? JOIN_THE_HORDE : JOIN_THE_ALLIANCE);

            WorldSession* session = bot->GetSession();
            session->HandleSelectFactionOpcode(packet);

            TC_LOG_INFO("playerbots", "%s Assigned to faction: %s", bot->GetName().c_str(), (bot->GetTeamId() ? "Alliance" : "Horde"));
        });        
    }
    if (bot->GetRace() == RACE_GOBLIN && bot->GetLevel() == 1)
    {
        bot->AddDelayedEvent(5000, [bot]()
        {
            // -- force finish quest
            auto it = bot->getRewardedQuests().find(25265);
            if (it == bot->getRewardedQuests().end())
            {
                bot->CompleteQuest(25265, true, true);
                bot->CompleteQuest(25266, true, true);
                bot->CompleteQuest(25267, true, true);
                bot->CompleteQuest(25268, true, true);
            }
        });
    }
    if (bot->GetRace() == RACE_WORGEN && bot->GetLevel() == 1)
    {
        bot->AddDelayedEvent(5000, [bot]()
        {
            // -- force finish quest
            auto it = bot->getRewardedQuests().find(26706);
            if (it == bot->getRewardedQuests().end())
            {
                bot->CompleteQuest(26706, true, true);
                bot->CompleteQuest(14434, true, true);
            }
        });
    }

    if (bot->GetClass() == CLASS_DEATH_KNIGHT && bot->GetLevel() == 55)
    {
        bot->AddDelayedEvent(5000, [bot]()
        {
            // -- force finish quest
            auto it = bot->getRewardedQuests().find(13188);
            if (it == bot->getRewardedQuests().end() && bot->GetTeam() == Team::ALLIANCE)
                bot->CompleteQuest(13188, true, true);
            it = bot->getRewardedQuests().find(13189);
            if (it == bot->getRewardedQuests().end() && bot->GetTeam() == Team::HORDE)
                bot->CompleteQuest(13189, true, true);
        });
    }

    /*if (sPlayerbotAIConfig->randomBotFixedLevel)
    {
        bot->SetPlayerFlag(PLAYER_FLAGS_NO_XP_GAIN);
    }
    else
    {
        bot->RemovePlayerFlag(PLAYER_FLAGS_NO_XP_GAIN);
    }*/
}

void RandomPlayerbotMgr::OnPlayerLogin(Player* player)
{
    uint32 botsNearby = 0;

    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator it = bots.begin(); it != bots.end(); ++it)
    {
        Player* const bot = it->second;
        if (player == bot /* || GET_PLAYERBOT_AI(player)*/)  // TEST
            continue;

        Cell playerCell(player->GetPositionX(), player->GetPositionY());
        Cell botCell(bot->GetPositionX(), bot->GetPositionY());

        // if (playerCell == botCell)
        // botsNearby++;

        Group* group = bot->GetGroup();
        if (!group)
            continue;

        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
            if (botAI && member == player && (!botAI->GetMaster() || GET_PLAYERBOT_AI(botAI->GetMaster())))
            {
                if (!bot->InBattleground())
                {
                    botAI->SetMaster(player);
                    botAI->ResetStrategies();
                    botAI->TellMaster("Hello");
                }

                break;
            }
        }
    }

    if (botsNearby > 100 && false)
    {
        WorldPosition botPos(player);

        if (!player->GetFactionTemplateEntry())
        {
            botPos.GetReachableRandomPointOnGround(player, sPlayerbotAIConfig->reactDistance * 2, true);
            player->TeleportTo(botPos);
        }
        else
        {
            /*std::vector<TravelDestination*> dests = sTravelMgr->getRpgTravelDestinations(player, true, true, 200000.0f);

            do
            {
                RpgTravelDestination* dest = (RpgTravelDestination*)dests[urand(0, dests.size() - 1)];
                CreatureTemplate const* cInfo = dest->GetCreatureTemplate();
                if (!cInfo)
                    continue;

                FactionTemplateEntry const* factionEntry = sFactionTemplateStore.LookupEntry(cInfo->faction);
                ReputationRank reaction = Unit::GetFactionReactionTo(player->GetFactionTemplateEntry(), factionEntry);

                if (reaction > REP_NEUTRAL && dest->nearestPoint(&botPos)->m_mapId == player->GetMapId())
                {
                    botPos = *dest->nearestPoint(&botPos);
                    break;
                }
            } while (true);*/
        }

        //player->TeleportTo(botPos);
    }

    if (!IsRandomBot(player))
    {
        std::lock_guard<std::mutex> lock(_playersMutex);
        if (std::find(_players.begin(), _players.end(), player) == _players.end())
        {
            _players.push_back(player);
            _realPlayersCount.store(static_cast<uint32>(_players.size()), std::memory_order_relaxed);
            TC_LOG_DEBUG("playerbots", "Including non-random bot player %s into random bot update", player->GetName().c_str());
        }
    }
}

void RandomPlayerbotMgr::OnPlayerLoginError(uint32 bot)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    SetEventValue(bot, "add", 0, 0);
    _currentBots.erase(std::remove(_currentBots.begin(), _currentBots.end(), bot), _currentBots.end());
    _currentBotSet.erase(bot);
}

Player* RandomPlayerbotMgr::GetRandomPlayer()
{
    std::lock_guard<std::mutex> lock(_playersMutex);
    if (_players.empty())
        return nullptr;

    uint32 index = std::rand() % _players.size();
    return _players[index];
}

Player* RandomPlayerbotMgr::GetPlayer(uint32 playerGuid)
{
    return GetPlayerBot(playerGuid);
}

void RandomPlayerbotMgr::PrintStats(uint32 requesterGuid)
{
    (void)requesterGuid;
    uint32 trackedBotCount = 0;
    {
        std::lock_guard<std::recursive_mutex> stateLock(_stateMutex);
        trackedBotCount = static_cast<uint32>(_currentBots.size());
    }
    uint32 realPlayerCount = 0;
    {
        std::lock_guard<std::mutex> lock(_playersMutex);
        realPlayerCount = static_cast<uint32>(_players.size());
    }

    TC_LOG_INFO("playerbots", "Random bot stats: online=%u tracked=%u realPlayers=%u loading=%u activity=%.2f%%",
        GetPlayerbotsCount(), trackedBotCount, realPlayerCount, static_cast<uint32>(GetBotLoadingCount()),
        getActivityPercentage());
}

double RandomPlayerbotMgr::GetBuyMultiplier(Player* bot)
{
    (void)bot;
    return 1.0;
}

double RandomPlayerbotMgr::GetSellMultiplier(Player* bot)
{
    (void)bot;
    return 1.0;
}

void RandomPlayerbotMgr::HandleCommand(uint32 /*type*/, std::string const& text, Player& fromPlayer,
    std::string /*channelName*/, Team /*team*/, uint32 /*lang*/)
{
    std::string result = HandleRemoteCommand(text);
    if (!result.empty())
        ChatHandler(fromPlayer.GetSession()).PSendSysMessage("%s", result.c_str());
}

bool RandomPlayerbotMgr::HandlePlayerbotConsoleCommand(ChatHandler* handler, char const* args)
{
    if (!handler || !args || !*args)
        return false;

    std::string result = HandleRemoteCommand(args);
    if (!result.empty())
        handler->PSendSysMessage("%s", result.c_str());
    return true;
}

std::string RandomPlayerbotMgr::HandleRemoteCommand(std::string request)
{
    std::istringstream in(request);
    std::string command;
    in >> command;
    if (command.empty())
        return "empty command";

    if (command == "stats")
    {
        PrintStats(0);
        return "stats logged";
    }

    static std::map<std::string, ConsoleCommandHandler> handlers;
    if (handlers.empty())
    {
        handlers["init"] = &RandomPlayerbotMgr::RandomizeFirst;
        handlers["upgrade"] = &RandomPlayerbotMgr::UpdateGearSpells;
        handlers["refresh"] = &RandomPlayerbotMgr::Refresh;
        handlers["teleport"] = &RandomPlayerbotMgr::RandomTeleportForLevel;
        handlers["rpg"] = &RandomPlayerbotMgr::RandomTeleport;
        handlers["revive"] = &RandomPlayerbotMgr::Revive;
        handlers["grind"] = &RandomPlayerbotMgr::RandomTeleport;
        handlers["change_strategy"] = &RandomPlayerbotMgr::ChangeStrategy;
        handlers["remove"] = &RandomPlayerbotMgr::Remove;
    }

    auto const handlerItr = handlers.find(command);
    if (handlerItr == handlers.end())
        return "unknown command";

    uint32 botGuid = 0;
    in >> botGuid;
    if (!botGuid)
        return "missing bot guid";

    Player* bot = GetPlayer(botGuid);
    if (!bot)
        return "bot not online";

    (this->*(handlerItr->second))(bot);
    return "ok";
}

void RandomPlayerbotMgr::Hotfix(Player* player, uint32 version)
{
    if (!player)
        return;

    uint32 current = GetValue(player, "hotfix_version");
    if (current >= version)
        return;

    if (!player->IsInWorld() || player->IsInCombat())
        return;

    UpdateGearSpells(player);
    Refresh(player);
    SetValue(player, "hotfix_version", version);
}

void RandomPlayerbotMgr::LogPlayerLocation()
{
    if (!sPlayerbotAIConfig->hasLog("player_location.csv"))
        return;

    time_t now = time(nullptr);
    if (_logPlayerLocationTimer && now < (_logPlayerLocationTimer + 30))
        return;
    _logPlayerLocationTimer = now;

    for (Player* player : GetPlayers())
    {
        if (!player || !player->IsInWorld())
            continue;

        std::ostringstream out;
        out << now << ","
            << player->GetGUID().GetCounter() << ","
            << player->GetMapId() << ","
            << player->GetPositionX() << ","
            << player->GetPositionY() << ","
            << player->GetPositionZ();
        sPlayerbotAIConfig->log("player_location.csv", out.str().c_str());
    }
}

void RandomPlayerbotMgr::DelayedFacingFix()
{
    time_t now = time(nullptr);
    if (_delayedFacingFixTimer && now < (_delayedFacingFixTimer + 5))
        return;
    _delayedFacingFixTimer = now;

    PlayerBotMap bots = GetPlayerBotsSnapshot();
    for (PlayerBotMap::const_iterator itr = bots.begin(); itr != bots.end(); ++itr)
    {
        Player* const bot = itr->second;
        if (!bot || !bot->IsInWorld() || bot->HasUnitState(UNIT_STATE_IN_FLIGHT) || bot->HasUnitState(UNIT_STATE_CASTING))
            continue;

        ObjectGuid targetGuid = bot->GetTarget();
        if (!targetGuid)
            continue;

        Unit* target = ObjectAccessor::GetUnit(*bot, targetGuid);
        if (!target || !target->IsInWorld())
            continue;

        float const meleeRange = bot->GetCombatReach() + target->GetCombatReach() + 1.5f;
        if (bot->GetExactDist2d(target) > meleeRange)
            continue;

        bot->SetFacingToObject(target);
    }
}

void RandomPlayerbotMgr::MirrorAh()
{
    if (!sPlayerbotAIConfig->hasLog("ah_mirror.csv"))
        return;

    static uint32 mirrorTimer = 0;
    if (mirrorTimer && GetMSTimeDiffToNow(mirrorTimer) < 60000)
        return;
    mirrorTimer = getMSTime();

    QueryResult result = CharacterDatabase.Query("SELECT COUNT(*) FROM auctionhouse");
    if (!result)
        return;

    uint32 count = result->Fetch()[0].GetUInt32();
    std::ostringstream out;
    out << getMSTime() << "," << count;
    sPlayerbotAIConfig->log("ah_mirror.csv", out.str().c_str());
}

void RandomPlayerbotMgr::MovePlayerBot(uint32 guid, PlayerbotHolder* newHolder)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    if (!newHolder || newHolder == this)
        return;

    ObjectGuid botGuid = ObjectGuid::Create<HighGuid::Player>(guid);
    if (!GetPlayerBot(botGuid))
        return;

    LogoutPlayerBot(botGuid);
    _currentBots.erase(std::remove(_currentBots.begin(), _currentBots.end(), guid), _currentBots.end());
    _currentBotSet.erase(guid);
    newHolder->AddPlayerBot(botGuid, 0);
}

bool RandomPlayerbotMgr::AddRandomBot(uint32 bot)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    if (GetPlayerBot(bot))
        return true;

    ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(bot);
    uint32 accountId = sCharacterCache->GetCharacterAccountIdByGuid(guid);
    if (!sPlayerbotAIConfig->IsInRandomAccountList(accountId))
    {
        TC_LOG_ERROR("playerbots", "Bot #%u login fail: not random bot account", bot);
        return false;
    }

    if (_currentBotSet.find(bot) == _currentBotSet.end())
    {
        _currentBots.push_back(bot);
        _currentBotSet.insert(bot);
    }

    SetEventValue(bot, "add", 1, urand(sPlayerbotAIConfig->minRandomBotInWorldTime, sPlayerbotAIConfig->maxRandomBotInWorldTime));
    SetEventValue(bot, "logout", 0, 0);
    SetEventValue(bot, "update", 1, urand(sPlayerbotAIConfig->minRandomBotReviveTime, sPlayerbotAIConfig->maxRandomBotReviveTime));
    ScheduleChangeStrategy(bot);

    if (!IsBotLoading(guid))
        AddPlayerBot(guid, 0);

    return true;
}

void RandomPlayerbotMgr::SaveCurTime()
{
    _eventTimeSyncTimer = time(nullptr);
    SetValue(uint32(0), "current_time", static_cast<uint32>(time(nullptr)));
}

void RandomPlayerbotMgr::SyncEventTimers()
{
    uint32 oldTime = GetValue(uint32(0), "current_time");
    if (oldTime)
    {
        uint32 curTime = static_cast<uint32>(time(nullptr));
        if (curTime > oldTime)
        {
            uint32 timeDiff = curTime - oldTime;
            CharacterDatabase.PExecute("UPDATE playerbots_random_bots SET `time` = `time` + %u WHERE owner = 0 AND bot <> 0", timeDiff);
        }
    }

    SaveCurTime();
}

void RandomPlayerbotMgr::PrepareAddclassCache()
{
    if (sPlayerbotAIConfig->randomBotAccounts.empty())
    {
        TC_LOG_WARN("playerbots", "Unable to prepare add class cache as accounts are empty");
        return;
    }
    int32 maxAccountId = sPlayerbotAIConfig->randomBotAccounts.back();
    int32 minIdx = 0;
    int32 minAccountId = sPlayerbotAIConfig->randomBotAccounts[minIdx];
    if (minAccountId < 0)
    {
        TC_LOG_ERROR("playerbots", "No available account for add class!");
    }
    int32 collected = 0;
    for (uint8 claz = CLASS_WARRIOR; claz <= CLASS_DRUID; claz++)
    {
        if (claz == 10)
            continue;
        QueryResult results = CharacterDatabase.PQuery(
            "SELECT guid, race FROM characters "
            "WHERE account >= %u AND account <= %u AND class = '%u' AND online = 0 AND "
            "guid NOT IN ( SELECT guid FROM guild_member ) "
            "ORDER BY account DESC",
            minAccountId, maxAccountId, claz);
        if (results)
        {
            do
            {
                Field* fields = results->Fetch();
                ObjectGuid guid = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
                uint32 race = fields[1].GetUInt32();
                bool isAlliance = race == 1 || race == 3 || race == 4 || race == 7 || race == 11;
                AddclassCache()[GetTeamClassIdx(isAlliance, claz)].push_back(guid);
                collected++;
            } while (results->NextRow());
        }
    }
    TC_LOG_INFO("playerbots", ">> %u characters collected for addclass command.", collected);
}

void RandomPlayerbotMgr::PrepareTeleportCache()
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    uint32 zone_count = 0, farm_spot_count = 0, city_count = 0;
    QueryResult results = WorldDatabase.PQuery("SELECT ZoneId, ZoneTyp, MinLevel, MaxLevel, TeamsDisabled, MapId, MinPlayers, MaxPlayers FROM playerbot_farming_zone");
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();

            uint32 zoneId = fields[0].GetUInt32();
            uint32 zoneType = fields[1].GetUInt32();
            uint32 minLevel = fields[2].GetUInt32();
            uint32 maxLevel = fields[3].GetUInt32();
            Team disabled = (Team)fields[4].GetUInt32();
            uint32 mapid = fields[5].GetUInt32();
            uint32 minPlayer = fields[6].GetUInt32();
            uint32 maxPlayer = fields[7].GetUInt32();

            if (zoneId == GM_ISLAND_ZONE_ID)
                continue;

            farm_zone new_zone
            {
                zoneId,     /* ZoneId */
                zoneType,   /* ZoneType */
                minLevel,   /* MinLevel */
                maxLevel,   /* MaxLevel */
                disabled,   /* TeamDisabled */
                mapid,      /* MapId */
                minPlayer,  /* MinPlayer */
                maxPlayer   /* MaxPlayer */
            };

            _farm_cache_data.emplace_back(std::move(new_zone));
            zone_count++;
        } while (results->NextRow());
    }

    results = WorldDatabase.PQuery("SELECT ZoneId, Id, MinLevel, MaxLevel, TeamsDisabled, X, Y, Z, Radius FROM playerbot_farming_spot");
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();

            uint32 zoneid = fields[0].GetUInt32();
            uint32 id = fields[1].GetUInt32();
            uint32 minLevel = fields[2].GetUInt32();
            uint32 maxLevel = fields[3].GetUInt32();
            Team disabled = (Team)fields[4].GetUInt32();
            float x = fields[5].GetFloat();
            float y = fields[6].GetFloat();
            float z = fields[7].GetFloat();
            uint32 radius = fields[8].GetUInt32();

            farm_spot new_spot
            {
                minLevel,   /* MinLevel */
                maxLevel,   /* MaxLevel */
                0,          /* MapId */
                x,          /* X */
                y,          /* Y */
                z           /* Z */
            };

            for (auto& zone_data : _farm_cache_data)
            {
                if (zone_data.zone_id == zoneid)
                {
                    new_spot.map_id = zone_data.map_id;
                    zone_data.spots.emplace_back(std::move(new_spot));
                    break;
                }
            }

            farm_spot_count++;
        } while (results->NextRow());
    }

    results = WorldDatabase.PQuery("SELECT ZoneId, CityId, MinLevel, MaxLevel, TeamsDisabled, X, Y, Z, Radius FROM playerbot_city");
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();

            uint32 ZoneId = fields[0].GetUInt32();
            uint32 city_id = fields[1].GetUInt32();
            uint32 minLevel = fields[2].GetUInt32();
            uint32 maxLevel = fields[3].GetUInt32();
            Team disabled = (Team)fields[4].GetUInt32();
            float x = fields[5].GetFloat();
            float y = fields[6].GetFloat();
            float z = fields[7].GetFloat();
            uint32 radius = fields[8].GetUInt32();
            uint32 map_id = UINT32_MAX;

            if (ZoneId == GM_ISLAND_ZONE_ID)
                continue;

            for (auto& zone_data : _farm_cache_data)
            {
                if (zone_data.zone_id == ZoneId)
                {
                    map_id = zone_data.map_id;
                    break;
                }
            }

            if (map_id == UINT32_MAX)
            {
                TC_LOG_ERROR("server.loading", "City {%u} in ZoneId {%u} is not found in zone cache", city_id, ZoneId);
                continue;
            }

            city new_city
            {
                ZoneId,     /* ZoneId */
                city_id,    /* CityId */
                map_id,     /* map_id */
                minLevel,   /* MinLevel */
                maxLevel,   /* MaxLevel */
                disabled,   /* TeamDisabled */
                x,          /* X */
                y,          /* Y */
                z,          /* Z */
            };

            _city_cache_data.emplace_back(std::move(new_city));
            city_count++;
        } while (results->NextRow());
    }
    results = WorldDatabase.PQuery("SELECT Zone, PathId, PointId, X, Y, Z FROM playerbot_zone_path");
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();

            uint32 ZoneId = fields[0].GetUInt32();
            uint32 PathId = fields[1].GetUInt32();
            uint32 PointId = fields[2].GetUInt32();
            float x = fields[3].GetFloat();
            float y = fields[4].GetFloat();
            float z = fields[5].GetFloat();

            farm_spot new_path
            {
                0,          /* MinLevel */
                0,          /* MaxLevel */
                0,          /* MapId */
                x,          /* X */
                y,          /* Y */
                z           /* Z */
            };

            for (auto& zone_data : _farm_cache_data)
            {
                if (zone_data.zone_id == ZoneId)
                {
                    new_path.map_id = zone_data.map_id;
                    new_path.min_level = zone_data.min_level;
                    new_path.max_level = zone_data.max_level;
                    zone_data.spots.emplace_back(std::move(new_path));
                    break;
                }
            }
        } while (results->NextRow());
    }

    _rpgLocsCacheLevel.clear();
    _rpgAreaNames.clear();
    uint32 const maxLevel = std::max<uint32>(1, std::min<uint32>(sPlayerbotAIConfig->randomBotMaxLevel, sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)));
    for (farm_zone const& zone : _farm_cache_data)
    {
        for (farm_spot const& spot : zone.spots)
        {
            uint32 const minLevel = std::max<uint32>(1, std::max(zone.min_level, spot.min_level));
            uint32 const maxSpotLevel = spot.max_level ? spot.max_level : zone.max_level;
            uint32 const maxAllowedLevel = std::min<uint32>(maxLevel, std::min(zone.max_level, maxSpotLevel));
            if (minLevel > maxAllowedLevel)
                continue;

            WorldLocation loc(zone.map_id, spot.x, spot.y, spot.z, 0.0f);
            uint32 zoneId = sMapMgr->GetZoneId(PHASEMASK_NORMAL, loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ());
            uint32 areaId = sMapMgr->GetAreaId(PHASEMASK_NORMAL, loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ());
            if (zoneId == GM_ISLAND_ZONE_ID || areaId == GM_ISLAND_ZONE_ID)
                continue;
            for (uint8 race = RACE_HUMAN; race < MAX_RACES; ++race)
            {
                if (!sChrRacesStore.LookupEntry(race))
                    continue;

                if (zone.team_disabled == Player::TeamForRace(race))
                    continue;

                for (uint32 level = minLevel; level <= maxAllowedLevel; ++level)
                    _rpgLocsCacheLevel[race][level].push_back(loc);
            }
        }
    }

    for (city const& cityData : _city_cache_data)
    {
        uint32 const minLevel = std::max<uint32>(1, cityData.min_level);
        uint32 const maxAllowedLevel = std::min<uint32>(maxLevel, cityData.max_level);
        if (minLevel > maxAllowedLevel)
            continue;

        WorldLocation loc(cityData.map_id, cityData.x, cityData.y, cityData.z, 0.0f);
        uint32 zoneId = sMapMgr->GetZoneId(PHASEMASK_NORMAL, loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ());
        uint32 areaId = sMapMgr->GetAreaId(PHASEMASK_NORMAL, loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ());
        if (zoneId == GM_ISLAND_ZONE_ID || areaId == GM_ISLAND_ZONE_ID)
            continue;
        for (uint8 race = RACE_HUMAN; race < MAX_RACES; ++race)
        {
            if (!sChrRacesStore.LookupEntry(race))
                continue;

            if (cityData.team_disabled == Player::TeamForRace(race))
                continue;

            for (uint32 level = minLevel; level <= maxAllowedLevel; ++level)
                _rpgLocsCacheLevel[race][level].push_back(loc);
        }
    }

    for (auto const& [race, levels] : _rpgLocsCacheLevel)
    {
        for (auto const& [level, locations] : levels)
        {
            std::vector<std::string>& areaNames = _rpgAreaNames[level][race];
            areaNames.reserve(locations.size());
            for (WorldLocation const& loc : locations)
                areaNames.push_back(WorldPosition(loc).getAreaName(true, true));
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u cache zone for %u farm spot", zone_count, farm_spot_count);
    TC_LOG_INFO("server.loading", ">> Loaded %u city zone", city_count);
}

void RandomPlayerbotMgr::RandomTeleportForLevel(Player* bot)
{
    if (!bot) return;

    if (sPlayerbotAIConfig->randomBotTeleportNearPlayer)
    {
        constexpr float nearPlayerTeleportRadius = 120.0f;
        std::vector<Player*> candidates;
        candidates.reserve(GetPlayers().size());

        for (Player* player : GetPlayers())
        {
            if (!player || player == bot || !player->IsInWorld() || player->IsBeingTeleported() || !player->IsAlive())
                continue;

            bool const playerOnGmIsland = player->GetZoneId() == GM_ISLAND_ZONE_ID || player->GetAreaId() == GM_ISLAND_ZONE_ID;
            bool const allowGmIslandTeleport = playerOnGmIsland && bot->GetGroup() && bot->GetGroup() == player->GetGroup();
            if (playerOnGmIsland && !allowGmIslandTeleport)
                continue;

            if (sRandomPlayerbotMgr->IsRandomBot(player))
                continue;

            if (player->GetMap()->IsDungeon() || player->GetMap()->IsRaid() || player->InBattleground())
                continue;

            if (IsInvalidTeleportAreaForBot(bot, player->GetZoneId(), player->GetAreaId()))
                continue;

            if (sPlayerbotAIConfig->randomBotTeleportNearPlayerMaxAmount > 0 &&
                sPlayerbotAIConfig->randomBotTeleportNearPlayerMaxAmountRadius > 0.0f)
            {
                uint32 nearbyBots = 0;
                PlayerBotMap bots = GetPlayerBotsSnapshot();
                for (PlayerBotMap::const_iterator itr = bots.begin(); itr != bots.end(); ++itr)
                {
                    Player* const rb = itr->second;
                    if (!rb || rb == bot || !rb->IsInWorld() || rb->IsBeingTeleported())
                        continue;

                    if (rb->GetMapId() != player->GetMapId())
                        continue;

                    if (rb->GetExactDist2d(player) <= sPlayerbotAIConfig->randomBotTeleportNearPlayerMaxAmountRadius)
                        ++nearbyBots;
                }

                if (nearbyBots >= sPlayerbotAIConfig->randomBotTeleportNearPlayerMaxAmount)
                    continue;
            }

            candidates.push_back(player);
        }

        if (!candidates.empty())
        {
            std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
            std::shuffle(candidates.begin(), candidates.end(), gen);

            for (Player* player : candidates)
            {
                bool const playerOnGmIsland = player->GetZoneId() == GM_ISLAND_ZONE_ID || player->GetAreaId() == GM_ISLAND_ZONE_ID;
                bool const allowGmIslandTeleport = playerOnGmIsland && bot->GetGroup() && bot->GetGroup() == player->GetGroup();
                for (uint8 attempt = 0; attempt < 8; ++attempt)
                {
                    float x = player->GetPositionX();
                    float y = player->GetPositionY();
                    float z = player->GetPositionZ();

                    float angle = frand(0.0f, 6.28318530718f);
                    float dist = frand(12.0f, nearPlayerTeleportRadius);
                    x += std::cos(angle) * dist;
                    y += std::sin(angle) * dist;

                    if (Map* map = sMapMgr->FindMap(player->GetMapId(), 0))
                    {
                        float groundZ = map->GetHeight(x, y, z);
                        if (groundZ != VMAP_INVALID_HEIGHT_VALUE)
                            z = groundZ;
                    }

                    uint32 zoneId = sMapMgr->GetZoneId(PHASEMASK_NORMAL, player->GetMapId(), x, y, z);
                    uint32 areaId = sMapMgr->GetAreaId(PHASEMASK_NORMAL, player->GetMapId(), x, y, z);
                    if (zoneId == GM_ISLAND_ZONE_ID || areaId == GM_ISLAND_ZONE_ID)
                    {
                        if (!allowGmIslandTeleport)
                            continue;
                    }

                    if (IsInvalidTeleportAreaForBot(bot, zoneId, areaId))
                        continue;

                    if (bot->TeleportTo(player->GetMapId(), x, y, z, bot->GetOrientation(), 0))
                    {
                        TC_LOG_INFO("playerbots", "Bot #%u <%s> teleported near real player <%s>: map{%u} %f:%f:%f",
                            bot->GetGUID().GetCounter(), bot->GetName().c_str(), player->GetName().c_str(),
                            player->GetMapId(), x, y, z);
                        return;
                    }
                }
            }
        }
    }

    // Teleport to city
    if (bot->GetLevel() >= 10 && urand(0, 100) < 0.25 * 100)
    {
        if (const auto city_data = GetCityForPlayer(bot))
        {
            Map* map = sMapMgr->FindMap(city_data->map_id, 0);

            bot->TeleportTo(city_data->map_id, city_data->x, city_data->y, city_data->z, 0.0f, 0);
            TC_LOG_INFO("playerbots", "Bot #%u <%s> teleported to City: map{%u} %f:%f:%f", bot->GetGUID().GetCounter(), bot->GetName().c_str(), city_data->map_id, city_data->x, city_data->y, city_data->z);
        }
        else if (const auto farm_spot = GetFarmZoneForPlayer(bot))
        {
            Map* map = sMapMgr->FindMap(farm_spot->map_id, 0);

            bot->TeleportTo(farm_spot->map_id, farm_spot->x, farm_spot->y, farm_spot->z, 0.0f, 0);
            TC_LOG_INFO("playerbots", "Bot #%u <%s> teleported to FarmSpot: map{%u} %f:%f:%f", bot->GetGUID().GetCounter(), bot->GetName().c_str(), farm_spot->map_id, farm_spot->x, farm_spot->y, farm_spot->z);
        }
    }
    else
    {
        if (const auto farm_zone = GetFarmZoneForPlayer(bot))
        {
            Map* map = sMapMgr->FindMap(farm_zone->map_id, 0);

            bot->TeleportTo(farm_zone->map_id, farm_zone->x, farm_zone->y, farm_zone->z, 0.0f, 0);
            TC_LOG_INFO("playerbots", "Bot #%u <%s> teleported to FarmZone: map{%u} %f:%f:%f", bot->GetGUID().GetCounter(), bot->GetName().c_str(), farm_zone->map_id, farm_zone->x, farm_zone->y, farm_zone->z);
        }
    }
}

void RandomPlayerbotMgr::RandomTeleport(Player* bot)
{
    RandomTeleportForLevel(bot);
}

std::vector<std::pair<uint32, uint32>> RandomPlayerbotMgr::RpgLocationsNear(WorldLocation pos,
    std::map<uint32, std::map<uint32, std::vector<std::string>>> const& areaNames, uint32 radius)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    std::vector<std::pair<uint32, uint32>> results;
    float minDist = std::numeric_limits<float>::max();
    WorldPosition areaPos(pos);
    std::string hasZone = "-";
    std::string wantZone = areaPos.getAreaName(true, true);

    for (uint32 level = 1; level < sPlayerbotAIConfig->randomBotMaxLevel + 1; ++level)
    {
        for (uint32 race = RACE_HUMAN; race < MAX_RACES; ++race)
        {
            auto raceItr = _rpgLocsCacheLevel.find(race);
            if (raceItr == _rpgLocsCacheLevel.end())
                continue;

            auto levelItr = raceItr->second.find(level);
            if (levelItr == raceItr->second.end())
                continue;

            auto areaLevelItr = areaNames.find(level);
            if (areaLevelItr == areaNames.end())
                continue;

            auto areaRaceItr = areaLevelItr->second.find(race);
            if (areaRaceItr == areaLevelItr->second.end())
                continue;

            std::vector<WorldLocation> const& locations = levelItr->second;
            std::vector<std::string> const& names = areaRaceItr->second;
            size_t const compareCount = std::min(locations.size(), names.size());
            for (size_t i = 0; i < compareCount; ++i)
            {
                std::string const& currentZone = names[i];
                if (currentZone != wantZone && hasZone == wantZone)
                    continue;

                if (currentZone == wantZone && hasZone != wantZone)
                    minDist = std::numeric_limits<float>::max();

                float dist = WorldPosition(pos).fDist(WorldPosition(locations[i]));
                if (dist > radius || dist > minDist)
                    continue;

                if (dist < minDist)
                    results.clear();

                results.emplace_back(race, level);
                hasZone = currentZone;
                minDist = dist;
            }
        }
    }

    return results;
}

void RandomPlayerbotMgr::RandomTeleportForRpg(Player* bot, bool activeOnly)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    if (!bot)
        return;

    uint32 const race = bot->GetRace();
    uint32 const level = bot->GetLevel();
    auto raceItr = _rpgLocsCacheLevel.find(race);
    if (raceItr == _rpgLocsCacheLevel.end())
    {
        RandomTeleportForLevel(bot);
        return;
    }

    auto levelItr = raceItr->second.find(level);
    if (levelItr == raceItr->second.end() || levelItr->second.empty())
    {
        RandomTeleportForLevel(bot);
        return;
    }

    std::vector<WorldLocation> const& locations = levelItr->second;
    auto tryTeleport = [bot](WorldLocation const& location, bool allowGmIslandTeleport) -> bool
    {
        uint32 zoneId = sMapMgr->GetZoneId(PHASEMASK_NORMAL, location.GetMapId(), location.GetPositionX(), location.GetPositionY(), location.GetPositionZ());
        uint32 areaId = sMapMgr->GetAreaId(PHASEMASK_NORMAL, location.GetMapId(), location.GetPositionX(), location.GetPositionY(), location.GetPositionZ());
        if ((zoneId == GM_ISLAND_ZONE_ID || areaId == GM_ISLAND_ZONE_ID) && !allowGmIslandTeleport)
            return false;

        float x = location.GetPositionX();
        float y = location.GetPositionY();
        float z = location.GetPositionZ();
        if (Map* map = sMapMgr->FindMap(location.GetMapId(), 0))
        {
            float groundZ = map->GetHeight(x, y, z);
            if (groundZ != VMAP_INVALID_HEIGHT_VALUE)
                z = groundZ;
        }

        return bot->TeleportTo(location.GetMapId(), x, y, z, bot->GetOrientation(), 0);
    };

    std::vector<Player*> activePlayers;
    activePlayers.reserve(GetPlayers().size());
    for (Player* player : GetPlayers())
    {
        if (!player || player == bot || !player->IsInWorld() || player->IsBeingTeleported() || !player->IsAlive())
            continue;

        bool const playerOnGmIsland = player->GetZoneId() == GM_ISLAND_ZONE_ID || player->GetAreaId() == GM_ISLAND_ZONE_ID;
        bool const allowGmIslandTeleport = playerOnGmIsland && bot->GetGroup() && bot->GetGroup() == player->GetGroup();
        if (playerOnGmIsland && !allowGmIslandTeleport)
            continue;

        if (IsRandomBot(player))
            continue;

        if (player->GetMap()->IsDungeon() || player->GetMap()->IsRaid() || player->InBattleground())
            continue;

        activePlayers.push_back(player);
    }

    if (!activePlayers.empty())
    {
        std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
        std::shuffle(activePlayers.begin(), activePlayers.end(), gen);

        for (Player* player : activePlayers)
        {
            WorldLocation playerLocation(
                player->GetMapId(),
                player->GetPositionX(),
                player->GetPositionY(),
                player->GetPositionZ(),
                player->GetOrientation());
            std::vector<std::pair<uint32, uint32>> ranges = RpgLocationsNear(playerLocation, _rpgAreaNames);
            if (std::find(ranges.begin(), ranges.end(), std::make_pair(race, level)) == ranges.end())
                continue;

            float bestDist = std::numeric_limits<float>::max();
            WorldLocation const* bestLocation = nullptr;
            WorldPosition playerPos(playerLocation);
            for (WorldLocation const& loc : locations)
            {
                if (loc.GetMapId() != playerLocation.GetMapId())
                    continue;

                float dist = playerPos.fDist(WorldPosition(loc));
                if (dist > 2000.0f || dist >= bestDist)
                    continue;

                bestDist = dist;
                bestLocation = &loc;
            }

            bool const playerOnGmIsland = player->GetZoneId() == GM_ISLAND_ZONE_ID || player->GetAreaId() == GM_ISLAND_ZONE_ID;
            bool const allowGmIslandTeleport = playerOnGmIsland && bot->GetGroup() && bot->GetGroup() == player->GetGroup();
            if (bestLocation && tryTeleport(*bestLocation, allowGmIslandTeleport))
            {
                TC_LOG_INFO("playerbots", "Bot #%u <%s> RPG teleported near player <%s>: map{%u} %f:%f:%f",
                    bot->GetGUID().GetCounter(), bot->GetName().c_str(), player->GetName().c_str(),
                    bestLocation->GetMapId(), bestLocation->GetPositionX(), bestLocation->GetPositionY(),
                    bestLocation->GetPositionZ());
                return;
            }
        }
    }

    if (activeOnly)
        return;

    std::vector<WorldLocation> shuffledLocations = locations;
    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(shuffledLocations.begin(), shuffledLocations.end(), gen);
    for (WorldLocation const& location : shuffledLocations)
    {
        if (tryTeleport(location, false))
        {
            TC_LOG_INFO("playerbots", "Bot #%u <%s> RPG teleported: map{%u} %f:%f:%f",
                bot->GetGUID().GetCounter(), bot->GetName().c_str(), location.GetMapId(), location.GetPositionX(),
                location.GetPositionY(), location.GetPositionZ());
            return;
        }
    }

    RandomTeleportForLevel(bot);
}

uint32 RandomPlayerbotMgr::GetZoneLevel(uint16 mapId, float teleX, float teleY, float teleZ)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    uint32 result = 0;

    for (farm_zone const& zone : _farm_cache_data)
    {
        if (zone.map_id != mapId)
            continue;

        for (farm_spot const& spot : zone.spots)
        {
            float dx = teleX - spot.x;
            float dy = teleY - spot.y;
            float dz = teleZ - spot.z;
            float distSq = dx * dx + dy * dy + dz * dz;
            if (distSq > (500.0f * 500.0f))
                continue;

            result = std::max(result, std::max(spot.max_level, zone.max_level));
        }
    }

    if (!result)
    {
        for (farm_zone const& zone : _farm_cache_data)
        {
            if (zone.map_id == mapId)
                result = std::max(result, zone.max_level);
        }
    }

    if (!result)
        result = sPlayerbotAIConfig->randomBotMaxLevel;

    return result;
}

void RandomPlayerbotMgr::PrintTeleportCache()
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    uint32 zoneCount = static_cast<uint32>(_farm_cache_data.size());
    uint32 cityCount = static_cast<uint32>(_city_cache_data.size());
    uint32 spotCount = 0;
    for (farm_zone const& zone : _farm_cache_data)
        spotCount += static_cast<uint32>(zone.spots.size());

    TC_LOG_INFO("playerbots", "Teleport cache: %u zones, %u spots, %u cities, %u named locations",
        zoneCount, spotCount, cityCount, static_cast<uint32>(_namedLocations.size()));
}

RandomPlayerbotMgr::city const* RandomPlayerbotMgr::GetCityForPlayer(Player* player)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    if (_city_cache_data.empty()) return nullptr;

    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(_city_cache_data.begin(), _city_cache_data.end(), gen);

    for (const auto& city_data : _city_cache_data)
    {
        if (player->GetLevel() < city_data.min_level || player->GetLevel() > city_data.max_level || city_data.team_disabled == player->GetTeam())
            continue;

        uint32 zoneId = sMapMgr->GetZoneId(PHASEMASK_NORMAL, city_data.map_id, city_data.x, city_data.y, city_data.z);
        uint32 areaId = sMapMgr->GetAreaId(PHASEMASK_NORMAL, city_data.map_id, city_data.x, city_data.y, city_data.z);
        if (zoneId == GM_ISLAND_ZONE_ID || areaId == GM_ISLAND_ZONE_ID || IsInvalidTeleportAreaForBot(player, zoneId, areaId))
            continue;

        return &city_data;
    }

    return nullptr;
}

RandomPlayerbotMgr::farm_spot const* RandomPlayerbotMgr::GetFarmZoneForPlayer(Player* player)
{
    std::lock_guard<std::recursive_mutex> lock(_stateMutex);
    if (_farm_cache_data.empty()) return nullptr;

    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(_farm_cache_data.begin(), _farm_cache_data.end(), gen);

    for (auto& zone : _farm_cache_data)
    {
        if (zone.zone_id == GM_ISLAND_ZONE_ID)
            continue;

        if (player->GetLevel() < zone.min_level || player->GetLevel() > zone.max_level || zone.team_disabled == player->GetTeam())
            continue;

        if (IsInvalidTeleportAreaForBot(player, zone.zone_id, zone.zone_id))
            continue;

        if (zone.max_player != 0)
        {
            uint32 playercount = 0;
            for (const auto& _internal_player : GetPlayers())
            {
                if (_internal_player && _internal_player->IsInWorld() && !_internal_player->IsBeingTeleported() && _internal_player->GetZoneId() == zone.zone_id && _internal_player != player)
                    ++playercount;
            }

            PlayerBotMap bots = GetPlayerBotsSnapshot();
            for (PlayerBotMap::const_iterator itr = bots.begin(); itr != bots.end(); ++itr)
            {
                Player* const bot = itr->second;
                if (!bot || bot == player || !bot->IsInWorld() || bot->IsBeingTeleported()
                    || bot->GetZoneId() != zone.zone_id) continue;

                ++playercount;
            }

            if (zone.max_player <= playercount)
            {
                TC_LOG_DEBUG("playerbots", "Zone {%u} has too many players for: %s {%u} : %u/%u", zone.zone_id, player->GetName().c_str(), player->GetLevel(), playercount, zone.max_player);

                continue;
            }
        }

        auto& spots = zone.spots;
        std::shuffle(spots.begin(), spots.end(), gen);
        for (auto& spot : spots)
        {
            uint32 zoneId = sMapMgr->GetZoneId(PHASEMASK_NORMAL, spot.map_id, spot.x, spot.y, spot.z);
            uint32 areaId = sMapMgr->GetAreaId(PHASEMASK_NORMAL, spot.map_id, spot.x, spot.y, spot.z);
            if (zoneId == GM_ISLAND_ZONE_ID || areaId == GM_ISLAND_ZONE_ID || IsInvalidTeleportAreaForBot(player, zoneId, areaId))
                continue;

            if (player->GetLevel() >= spot.min_level && player->GetLevel() <= spot.max_level)
            {
                // -- 
                TC_LOG_DEBUG("playerbots", "Farm spot found for : %s - level: %u", player->GetName().c_str(), player->GetLevel());
                return &spot;
            }
        }
    }

    TC_LOG_WARN("playerbots", "No valid zone farm found for %s level: %u", player->GetName().c_str(), player->GetLevel());
    return nullptr;
}

void RandomPlayerbotMgr::PushMetric(botPerformanceMetric& metric, uint32 bot, float value, uint32 maxNum) const
{
    std::list<float>& values = metric[bot];
    values.push_back(value);

    if (values.size() > maxNum)
        values.pop_front();
}

float RandomPlayerbotMgr::GetMetricDelta(botPerformanceMetric& metric) const
{
    float deltaMetric = 0.0f;
    for (auto& botMetric : metric)
    {
        std::list<float>& values = botMetric.second;
        if (values.size() > 1)
            deltaMetric += (values.back() - values.front()) / values.size();
    }

    if (metric.empty())
        return 0.0f;

    return deltaMetric / metric.size();
}

