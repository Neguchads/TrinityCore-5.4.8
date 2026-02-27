

#ifndef _PLAYERBOT_PLAYERbotAICONFIG_H
#define _PLAYERBOT_PLAYERbotAICONFIG_H

#include <array>
#include <mutex>

#include "Common.h"
#include "DBCEnums.h"
#include "SharedDefines.h"

enum class HealingManaEfficiency : uint8
{
    VERY_LOW = 1,
    LOW = 2,
    MEDIUM = 4,
    HIGH = 8,
    VERY_HIGH = 16,
    SUPERIOR = 32
};

enum class BotCheatMask : uint32
{
    none   = 0,
    food   = 1 << 0,
    taxi   = 1 << 1,
    gold   = 1 << 2,
    health = 1 << 3,
    mana   = 1 << 4,
    power  = 1 << 5,
    raid   = 1 << 6,
    maxMask = 1 << 7
};

class PlayerbotAIConfig
{
public:
    PlayerbotAIConfig() {};
    static PlayerbotAIConfig* instance()
    {
        static PlayerbotAIConfig instance;
        return &instance;
    }

    bool Initialize();
    
    bool IsPvpProhibited(uint32 zoneId, uint32 areaId);
    bool IsInPvpProhibitedZone(uint32 id);
    bool IsInPvpProhibitedArea(uint32 id);

    bool enabled, perfMonEnabled;
    uint32 globalCoolDown, reactDelay, maxWaitForMove, disableMoveSplinePath, maxMovementSearchTime, expireActionTime,
        dispelAuraDuration, passiveDelay, repeatDelay, errorDelay, rpgDelay, sitDelay, returnDelay, lootDelay;
    bool dynamicReactDelay;
    bool fastReactInBG;
    float sightDistance, spellDistance, reactDistance, grindDistance, lootDistance, shootDistance, fleeDistance,
        tooCloseDistance, meleeDistance, followDistance, whisperDistance, contactDistance, aoeRadius, rpgDistance,
        targetPosRecalcDistance, farDistance, healDistance, aggroDistance;
    uint32 criticalHealth, lowHealth, mediumHealth, almostFullHealth;
    uint32 lowMana, mediumMana, highMana;
    bool autoSaveMana;
    uint32 saveManaThreshold;
    bool fleeingEnabled;
    bool autoAvoidAoe;
    float maxAoeAvoidRadius;
    std::set<uint32> aoeAvoidSpellWhitelist;
    bool tellWhenAvoidAoe;

    int32 maintenanceCommand;
    BotCheatMask botCheatMask;
    uint32 useGroundMountAtMinLevel;
    uint32 useFastGroundMountAtMinLevel;
    uint32 useFlyMountAtMinLevel;
    uint32 useFastFlyMountAtMinLevel;
    uint32 minEnchantingBotLevel;
    std::vector<uint32> randomBotSpellIds;
    bool randomBotPreQuests;
    std::vector<uint32> randomBotQuestIds;

    bool altMaintenanceBags;
    bool altMaintenanceFood;
    bool altMaintenanceReagents;
    bool altMaintenanceConsumables;
    bool altMaintenancePotions;
    bool altMaintenanceMounts;
    bool altMaintenanceSkills;
    bool altMaintenanceClassSpells;
    bool altMaintenanceAvailableSpells;
    bool altMaintenanceSpecialSpells;
    bool altMaintenanceTalentTree;
    bool altMaintenanceGlyphs;
    bool altMaintenanceGemsEnchants;
    bool altMaintenancePet;
    bool altMaintenancePetTalents;
    bool altMaintenanceReputation;

    uint32 openGoSpell;
    bool randomBotAutologin, disableRandomLevels, randomGearUpgradeEnabled, syncLevelWithPlayers;
    uint32 syncLevelMaxAbove, syncLevelNoPlayer;
    uint32 randomBotMaxLevel, randomBotMinLevel;
    float randomBotMaxLevelChance;
    uint32 randombotStartingLevel;
    uint32 minRandomBots, maxRandomBots, maxAddedBotsPerClass, maxAddedBots;
    uint32 randomBotUpdateInterval, randomBotCountChangeMinInterval, randomBotCountChangeMaxInterval;
    uint32 minRandomBotRandomizeTime, maxRandomBotRandomizeTime;
    uint32 minRandomBotTeleportInterval, maxRandomBotTeleportInterval;
    bool randomBotTeleportNearPlayer;
    uint32 randomBotTeleportNearPlayerMaxAmount;
    float randomBotTeleportNearPlayerMaxAmountRadius;
    uint32 minRandomBotInWorldTime, maxRandomBotInWorldTime;
    uint32 minRandomBotReviveTime, maxRandomBotReviveTime;
    uint32 randomBotsPerInterval;
    uint32 randomBotsMaxLoginsPerInterval;
    uint32 randomBotsDatabasePingInterval;
    uint32 randomBotsDatabaseDelayThreshold;
    float randomChangeMultiplier;
    bool randomBotLoginAtStartup;
    bool randomBotTalk;
    bool randomBotEmote;
    bool enableGreet;
    bool randomBotSayWithoutMaster;
    bool randomBotTimedLogout;
    bool randomBotTimedOffline;
    bool randomBotLoginWithPlayer;
    bool asyncBotLogin;
    bool preloadHolders;
    uint32 freeRoomForNonSpareBots;
    uint32 loginBotsNearPlayerRange;
    std::vector<std::string> defaultLoginCriteria;
    std::vector<std::vector<std::string>> loginCriteria;
    uint32 classRaceProbabilityTotal;
    uint32 classRaceProbability[MAX_CLASSES][MAX_RACES];
    bool useFixedClassRaceCounts;
    uint32 levelProbability[DEFAULT_MAX_LEVEL + 1];
    bool botTextLocaleWeightsEnabled;
    std::array<uint32, MAX_LOCALES> botTextLocaleWeights;
    std::string randomBotAccountPrefix;
    uint32 randomBotAccountCount;
    bool randomBotRandomPassword;
    std::vector<uint32> randomBotAccounts;
    bool randomBotJoinBG;
    bool randomBotAutoJoinBG;
    bool randomBotJoinLfg;

    // CMaNGOS parity: when enabled, DbStore is only saved via explicit chat commands (save ai).
    bool explicitDbStoreSave = false;

    // Guild automation (random bots).
    uint32 randomBotGuildCount = 0;
    bool deleteRandomBotGuilds = false;
    uint32 guildMaxBotLimit = 1000;
    bool randomBotGuildNearby = true;
    bool randomBotInvitePlayer = true;
    bool inviteChat = false;
    bool randomBotFormGuild = true;

    // Runtime list of guild ids that were created/managed for random bots.
    std::vector<uint32> randomBotGuilds;

    uint32 llmEnabled = 0; // 0=disabled, 1=enabled with "ai chat" strategy
    std::string llmExpansionName;
    std::string llmApiEndpoint;
    std::string llmApiKey;
    std::string llmApiJson;
    uint32 llmContextLength = 4096;
    uint32 llmGenerationTimeout = 15;
    uint32 llmMaxSimultaniousGenerations = 32;
    std::string llmPrePrompt;
    std::string llmPrompt;
    std::string llmPostPrompt;
    std::string llmResponseStartPattern;
    std::string llmResponseEndPattern;
    std::string llmResponseDeletePattern;
    std::string llmResponseSplitPattern;
    bool llmGlobalContext = false;
    uint32 llmBotToBotChatChance = 0;
    uint32 llmRpgAIChatChance = 100;
    std::set<uint32> llmBlockedReplyChannels;

    uint32 botActiveAlone;
    uint32 BotActiveAloneForceWhenInRadius;
    bool BotActiveAloneForceWhenInZone;
    bool BotActiveAloneForceWhenInMap;
    bool BotActiveAloneForceWhenIsFriend;
    bool BotActiveAloneForceWhenInGuild;
    uint32 diffWithPlayer;
    uint32 diffEmpty;
    bool disableBotOptimizations;
    bool disableActivityPriorities;
    bool botActiveAloneSmartScale;
    uint32 botActiveAloneSmartScaleDiffLimitfloor;
    uint32 botActiveAloneSmartScaleDiffLimitCeiling;
    uint32 botActiveAloneSmartScaleWhenMinLevel;
    uint32 botActiveAloneSmartScaleWhenMaxLevel;
    float respawnModNeutral;
    float respawnModHostile;
    uint32 respawnModThreshold;
    uint32 respawnModMax;
    bool respawnModForPlayerBots;
    bool respawnModForInstances;

    uint32 randomBotAutoJoinArenaBracket;
    std::string randomBotAutoJoinICBrackets;
    std::string randomBotAutoJoinEYBrackets;
    std::string randomBotAutoJoinAVBrackets;
    std::string randomBotAutoJoinABBrackets;
    std::string randomBotAutoJoinWSBrackets;
    uint32 randomBotAutoJoinBGICCount;
    uint32 randomBotAutoJoinBGEYCount;
    uint32 randomBotAutoJoinBGAVCount;
    uint32 randomBotAutoJoinBGABCount;
    uint32 randomBotAutoJoinBGWSCount;
    uint32 randomBotAutoJoinBGRatedArena2v2Count;
    uint32 randomBotAutoJoinBGRatedArena3v3Count;
    uint32 randomBotAutoJoinBGRatedArena5v5Count;

    std::string premadeSpecName[MAX_CLASSES][MAX_SPECIALIZATIONS - 1];
    std::vector<uint16> premadeSpecLink[MAX_CLASSES][MAX_SPECIALIZATIONS - 1];

    uint32 iterationsPerTick;

    std::string combatStrategies, nonCombatStrategies;
    std::string randomBotCombatStrategies, randomBotNonCombatStrategies;
    std::vector<uint32> pvpProhibitedZoneIds;
    std::vector<uint32> pvpProhibitedAreaIds;

    std::mutex m_logMtx;
    std::vector<std::string> allowedLogFiles;
    std::unordered_map<std::string, std::pair<FILE*, bool>> logFiles;

    // METHODS
    bool IsInRandomAccountList(uint32 id);

    std::string const GetTimestampStr();
    bool hasLog(std::string const fileName)
    {
        return std::find(allowedLogFiles.begin(), allowedLogFiles.end(), fileName) != allowedLogFiles.end();
    };
    bool openLog(std::string const fileName, char const* mode = "a");
    bool isLogOpen(std::string const fileName)
    {
        auto it = logFiles.find(fileName);
        return it != logFiles.end() && it->second.second;
    }
    void log(std::string const fileName, char const* str, ...);
};

#define sPlayerbotAIConfig PlayerbotAIConfig::instance()

#endif
