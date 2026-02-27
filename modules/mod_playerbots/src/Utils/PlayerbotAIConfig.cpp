/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#include "PlayerbotAIConfig.h"

#include <cctype>
#include <cstring>
#include <iostream>

#include "Config.h"
#include "Helper.h"
#include "ObjectMgr.h"
#include "Playerbots.h"
#include "PlayerbotTextMgr.h"
#include "PlayerbotHelpMgr.h"
#include "RandomPlayerbotMgr.h"
#include "Factory/RandomPlayerbotFactory.h"

template <class T>
void LoadList(std::string const value, T& list)
{
    std::vector<std::string> ids = split(value, ',');
    for (std::vector<std::string>::iterator i = ids.begin(); i != ids.end(); i++)
    {
        uint32 id = atoi((*i).c_str());
        // if (!id)
        //     continue;
        list.push_back(id);
    }
}

template <class T>
void LoadSet(std::string const value, T& set)
{
    std::vector<std::string> ids = split(value, ',');
    for (std::vector<std::string>::iterator i = ids.begin(); i != ids.end(); i++)
    {
        uint32 id = atoi((*i).c_str());
        // if (!id)
        //     continue;
        set.insert(id);
    }
}

template <class T>
void LoadListString(std::string const value, T& list)
{
    std::vector<std::string> strings = split(value, ',');
    for (std::vector<std::string>::iterator i = strings.begin(); i != strings.end(); i++)
    {
        std::string const string = *i;
        if (string.empty())
            continue;

        list.push_back(string);
    }
}

bool PlayerbotAIConfig::Initialize()
{
    TC_LOG_INFO("server.loading", "Initializing AI Playerbots");
    enabled = sConfigMgr->GetBoolDefault("AiPlayerbot.Enabled", true);
    if (!enabled)
    {
        TC_LOG_INFO("server.loading", "AI Playerbots is Disabled in playerbots.conf");
        return false;
    }

    perfMonEnabled = sConfigMgr->GetBoolDefault("AiPlayerbot.PerfMonEnabled", false);
    globalCoolDown = sConfigMgr->GetIntDefault("AiPlayerbot.GlobalCooldown", 1500);
    maxWaitForMove = sConfigMgr->GetIntDefault("AiPlayerbot.MaxWaitForMove", 5000);
    disableMoveSplinePath = sConfigMgr->GetIntDefault("AiPlayerbot.DisableMoveSplinePath", 0);
    maxMovementSearchTime = sConfigMgr->GetIntDefault("AiPlayerbot.MaxMovementSearchTime", 3);
    expireActionTime = sConfigMgr->GetIntDefault("AiPlayerbot.ExpireActionTime", 5000);
    dispelAuraDuration = sConfigMgr->GetIntDefault("AiPlayerbot.DispelAuraDuration", 700);
    reactDelay = sConfigMgr->GetIntDefault("AiPlayerbot.ReactDelay", 500);
    dynamicReactDelay = sConfigMgr->GetBoolDefault("AiPlayerbot.DynamicReactDelay", false);
    fastReactInBG = sConfigMgr->GetBoolDefault("AiPlayerbot.FastReactInBG", true);
    passiveDelay = sConfigMgr->GetIntDefault("AiPlayerbot.PassiveDelay", 10000);
    repeatDelay = sConfigMgr->GetIntDefault("AiPlayerbot.RepeatDelay", 5000);
    errorDelay = sConfigMgr->GetIntDefault("AiPlayerbot.ErrorDelay", 5000);
    rpgDelay = sConfigMgr->GetIntDefault("AiPlayerbot.RpgDelay", 10000);
    sitDelay = sConfigMgr->GetIntDefault("AiPlayerbot.SitDelay", 20000);
    returnDelay = sConfigMgr->GetIntDefault("AiPlayerbot.ReturnDelay", 2000);
    lootDelay = sConfigMgr->GetIntDefault("AiPlayerbot.LootDelay", 1000);

    farDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.FarDistance", 20.0f);
    sightDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.SightDistance", 60.0f);
    spellDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.SpellDistance", 26.0f);
    shootDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.ShootDistance", 26.0f);
    healDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.HealDistance", 25.0f);
    lootDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.LootDistance", 15.0f);
    fleeDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.FleeDistance", 7.5f);
    aggroDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.AggroDistance", 22.0f);
    tooCloseDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.TooCloseDistance", 5.0f);
    meleeDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.MeleeDistance", 0.75f);
    followDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.FollowDistance", 1.5f);
    whisperDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.WhisperDistance", 6000.0f);
    contactDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.ContactDistance", 0.5f);
    aoeRadius = sConfigMgr->GetFloatDefault("AiPlayerbot.AoeRadius", 5.0f);
    rpgDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.RpgDistance", 200.0f);
    grindDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.GrindDistance", 75.0f);
    reactDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.ReactDistance", 150.0f);
    targetPosRecalcDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.TargetPosRecalcDistance", 0.1f);

    criticalHealth = sConfigMgr->GetIntDefault("AiPlayerbot.CriticalHealth", 20);
    lowHealth = sConfigMgr->GetIntDefault("AiPlayerbot.LowHealth", 45);
    mediumHealth = sConfigMgr->GetIntDefault("AiPlayerbot.MediumHealth", 65);
    almostFullHealth = sConfigMgr->GetIntDefault("AiPlayerbot.AlmostFullHealth", 85);
    lowMana = sConfigMgr->GetIntDefault("AiPlayerbot.LowMana", 15);
    mediumMana = sConfigMgr->GetIntDefault("AiPlayerbot.MediumMana", 40);
    highMana = sConfigMgr->GetIntDefault("AiPlayerbot.HighMana", 65);
    autoSaveMana = sConfigMgr->GetBoolDefault("AiPlayerbot.AutoSaveMana", true);
    saveManaThreshold = sConfigMgr->GetIntDefault("AiPlayerbot.SaveManaThreshold", 60);
    fleeingEnabled = sConfigMgr->GetBoolDefault("AiPlayerbot.FleeingEnabled", true);
    autoAvoidAoe = sConfigMgr->GetBoolDefault("AiPlayerbot.AutoAvoidAoe", true);
    maxAoeAvoidRadius = sConfigMgr->GetFloatDefault("AiPlayerbot.MaxAoeAvoidRadius", 15.0f);
    LoadSet<std::set<uint32>>(sConfigMgr->GetStringDefault("AiPlayerbot.AoeAvoidSpellWhitelist", "50759,57491,13810"),
        aoeAvoidSpellWhitelist);
    tellWhenAvoidAoe = sConfigMgr->GetBoolDefault("AiPlayerbot.TellWhenAvoidAoe", false);

    maintenanceCommand = sConfigMgr->GetIntDefault("AiPlayerbot.MaintenanceCommand", 1);
    botCheatMask = BotCheatMask::none;
    std::vector<std::string> botCheats;
    LoadListString<std::vector<std::string>>(sConfigMgr->GetStringDefault("AiPlayerbot.BotCheats", "food,taxi"),
        botCheats);
    for (std::string const& cheat : botCheats)
    {
        if (cheat == "food")
            botCheatMask = BotCheatMask(static_cast<uint32>(botCheatMask) | static_cast<uint32>(BotCheatMask::food));
        else if (cheat == "taxi")
            botCheatMask = BotCheatMask(static_cast<uint32>(botCheatMask) | static_cast<uint32>(BotCheatMask::taxi));
        else if (cheat == "gold")
            botCheatMask = BotCheatMask(static_cast<uint32>(botCheatMask) | static_cast<uint32>(BotCheatMask::gold));
        else if (cheat == "health")
            botCheatMask = BotCheatMask(static_cast<uint32>(botCheatMask) | static_cast<uint32>(BotCheatMask::health));
        else if (cheat == "mana")
            botCheatMask = BotCheatMask(static_cast<uint32>(botCheatMask) | static_cast<uint32>(BotCheatMask::mana));
        else if (cheat == "power")
            botCheatMask = BotCheatMask(static_cast<uint32>(botCheatMask) | static_cast<uint32>(BotCheatMask::power));
        else if (cheat == "raid")
            botCheatMask = BotCheatMask(static_cast<uint32>(botCheatMask) | static_cast<uint32>(BotCheatMask::raid));
    }

    useGroundMountAtMinLevel = sConfigMgr->GetIntDefault("AiPlayerbot.UseGroundMountAtMinLevel", 20);
    useFastGroundMountAtMinLevel = sConfigMgr->GetIntDefault("AiPlayerbot.UseFastGroundMountAtMinLevel", 40);
    useFlyMountAtMinLevel = sConfigMgr->GetIntDefault("AiPlayerbot.UseFlyMountAtMinLevel", 60);
    useFastFlyMountAtMinLevel = sConfigMgr->GetIntDefault("AiPlayerbot.UseFastFlyMountAtMinLevel", 70);
    minEnchantingBotLevel = sConfigMgr->GetIntDefault("AiPlayerbot.MinEnchantingBotLevel", 60);
    randomBotSpellIds.clear();
    LoadList<std::vector<uint32>>(sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotSpellIds", ""), randomBotSpellIds);

    altMaintenanceBags = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceBags", true);
    altMaintenanceFood = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceFood", true);
    altMaintenanceReagents = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceReagents", true);
    altMaintenanceConsumables = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceConsumables", true);
    altMaintenancePotions = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenancePotions", true);
    altMaintenanceMounts = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceMounts", true);
    altMaintenanceSkills = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceSkills", true);
    altMaintenanceClassSpells = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceClassSpells", true);
    altMaintenanceAvailableSpells = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceAvailableSpells", true);
    altMaintenanceSpecialSpells = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceSpecialSpells", true);
    altMaintenanceTalentTree = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceTalentTree", true);
    altMaintenanceGlyphs = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceGlyphs", true);
    altMaintenanceGemsEnchants = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceGemsEnchants", true);
    altMaintenancePet = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenancePet", true);
    altMaintenancePetTalents = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenancePetTalents", true);
    altMaintenanceReputation = sConfigMgr->GetBoolDefault("AiPlayerbot.AltMaintenanceReputation", true);

    iterationsPerTick = sConfigMgr->GetIntDefault("AiPlayerbot.IterationsPerTick", 100);

    LoadList<std::vector<uint32>>(
        sConfigMgr->GetStringDefault("AiPlayerbot.PvpProhibitedZoneIds",
            "2255,656,2361,2362,2363,976,35,2268,3425,392,541,1446,3828,3712,3738,3565,"
            "3539,3623,4152,3988,4658,4284,4418,4436,4275,4323,4395"),
        pvpProhibitedZoneIds);
    LoadList<std::vector<uint32>>(sConfigMgr->GetStringDefault("AiPlayerbot.PvpProhibitedAreaIds", "976,35"),
        pvpProhibitedAreaIds);
 
    randomBotAutologin = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotAutologin", true);
    randomBotAccountPrefix = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotAccountPrefix", "rndbot");
    randomBotAccountCount = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAccountCount", 200);
    randomBotJoinBG = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotJoinBG", true);
    randomBotAutoJoinBG = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotAutoJoinBG", false);
    randomBotJoinLfg = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotJoinLfg", true);

    explicitDbStoreSave = sConfigMgr->GetBoolDefault("AiPlayerbot.ExplicitDbStoreSave", false);

    randomBotGuildCount = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotGuildCount", 0);
    deleteRandomBotGuilds = sConfigMgr->GetBoolDefault("AiPlayerbot.DeleteRandomBotGuilds", false);
    guildMaxBotLimit = sConfigMgr->GetIntDefault("AiPlayerbot.GuildMaxBotLimit", 1000);
    randomBotGuildNearby = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotGuildNearby", true);
    randomBotInvitePlayer = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotInvitePlayer", true);
    inviteChat = sConfigMgr->GetBoolDefault("AiPlayerbot.InviteChat", false);
    randomBotFormGuild = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotFormGuild", true);

    llmEnabled = sConfigMgr->GetIntDefault("AiPlayerbot.LLMEnabled", 0);
    llmExpansionName = sConfigMgr->GetStringDefault("AiPlayerbot.LLMExpansionName", "Mists of Pandaria");
    llmApiEndpoint = sConfigMgr->GetStringDefault("AiPlayerbot.LLMApiEndpoint", "");
    llmApiKey = sConfigMgr->GetStringDefault("AiPlayerbot.LLMApiKey", "");
    llmApiJson = sConfigMgr->GetStringDefault("AiPlayerbot.LLMApiJson", "");
    llmContextLength = sConfigMgr->GetIntDefault("AiPlayerbot.LLMContextLength", 4096);
    llmGenerationTimeout = sConfigMgr->GetIntDefault("AiPlayerbot.LLMGenerationTimeout", 15);
    llmMaxSimultaniousGenerations = sConfigMgr->GetIntDefault("AiPlayerbot.LLMMaxSimultaniousGenerations", 32);
    llmPrePrompt = sConfigMgr->GetStringDefault("AiPlayerbot.LLMPrePrompt", "");
    llmPrompt = sConfigMgr->GetStringDefault("AiPlayerbot.LLMPrompt", "<receiver name>:<initial message>");
    llmPostPrompt = sConfigMgr->GetStringDefault("AiPlayerbot.LLMPostPrompt", "<bot name>:");
    llmResponseStartPattern = sConfigMgr->GetStringDefault("AiPlayerbot.LLMResponseStartPattern", R"((\"content\"\\s*:\\s*\")|(\"text\"\\s*:\\s*\"))");
    llmResponseEndPattern = sConfigMgr->GetStringDefault("AiPlayerbot.LLMResponseEndPattern", R"((\"))");
    llmResponseDeletePattern = sConfigMgr->GetStringDefault("AiPlayerbot.LLMResponseDeletePattern", R"((\\n|\\r|\\t))");
    llmResponseSplitPattern = sConfigMgr->GetStringDefault("AiPlayerbot.LLMResponseSplitPattern", R"(([^\r\n]+))");
    llmGlobalContext = sConfigMgr->GetBoolDefault("AiPlayerbot.LLMGlobalContext", false);
    llmBotToBotChatChance = sConfigMgr->GetIntDefault("AiPlayerbot.LLMBotToBotChatChance", 0);
    llmRpgAIChatChance = sConfigMgr->GetIntDefault("AiPlayerbot.LLMRpgAIChatChance", 100);
    llmBlockedReplyChannels.clear();
    LoadSet<std::set<uint32>>(sConfigMgr->GetStringDefault("AiPlayerbot.LLMBlockedReplyChannels", ""), llmBlockedReplyChannels);

    randomBotTalk = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotTalk", true);
    randomBotEmote = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotEmote", false);
    enableGreet = sConfigMgr->GetBoolDefault("AiPlayerbot.EnableGreet", false);
    randomBotSayWithoutMaster = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotSayWithoutMaster", false);
    randomBotTimedLogout = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotTimedLogout", true);
    randomBotTimedOffline = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotTimedOffline", false);
    randomBotLoginWithPlayer = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotLoginWithPlayer", false);
    asyncBotLogin = sConfigMgr->GetBoolDefault("AiPlayerbot.AsyncBotLogin", false);
    preloadHolders = sConfigMgr->GetBoolDefault("AiPlayerbot.PreloadHolders", false);
    freeRoomForNonSpareBots = sConfigMgr->GetIntDefault("AiPlayerbot.FreeRoomForNonSpareBots", 1);
    loginBotsNearPlayerRange = sConfigMgr->GetIntDefault("AiPlayerbot.LoginBotsNearPlayerRange", 1000);
    defaultLoginCriteria.clear();
    LoadListString<std::vector<std::string>>(
        sConfigMgr->GetStringDefault("AiPlayerbot.DefaultLoginCriteria1", "maxbots,spareroom,offline"),
        defaultLoginCriteria);
    loginCriteria.clear();
    for (uint8 i = 1; i <= 20; ++i)
    {
        std::string key = "AiPlayerbot.LoginCriteria." + std::to_string(i);
        std::vector<std::string> criteria;
        LoadListString<std::vector<std::string>>(sConfigMgr->GetStringDefault(key, ""), criteria);
        if (!criteria.empty())
            loginCriteria.push_back(criteria);
    }

    if (loginCriteria.empty())
    {
        loginCriteria.push_back({ "group" });
        loginCriteria.push_back({ "arena" });
        loginCriteria.push_back({ "bg" });
        loginCriteria.push_back({ "guild" });
        loginCriteria.push_back({ "logoff", "classrace", "level", "online" });
        loginCriteria.push_back({ "logoff", "classrace", "level" });
        loginCriteria.push_back({ "logoff", "classrace" });
    }

    for (uint32 level = 1; level <= DEFAULT_MAX_LEVEL; ++level)
        levelProbability[level] = sConfigMgr->GetIntDefault("AiPlayerbot.LevelProbability." + std::to_string(level), 100);

    classRaceProbabilityTotal = 0;
    useFixedClassRaceCounts = sConfigMgr->GetBoolDefault("AiPlayerbot.ClassRace.UseFixedClassRaceCounts", false);
    std::memset(classRaceProbability, 0, sizeof(classRaceProbability));

    for (uint32 race = 1; race < MAX_RACES; ++race)
    {
        int32 raceProb = sConfigMgr->GetIntDefault("AiPlayerbot.ClassRaceProb.0." + std::to_string(race), 100);
        for (uint32 cls = 1; cls < MAX_CLASSES; ++cls)
            classRaceProbability[cls][race] = raceProb > 0 ? static_cast<uint32>(raceProb) : 0;
    }

    for (uint32 cls = 1; cls < MAX_CLASSES; ++cls)
    {
        int32 classProb = sConfigMgr->GetIntDefault("AiPlayerbot.ClassRaceProb." + std::to_string(cls), -1);
        if (classProb < 0)
            continue;

        for (uint32 race = 1; race < MAX_RACES; ++race)
            classRaceProbability[cls][race] = classProb > 0 ? static_cast<uint32>(classProb) : 0;
    }

    for (uint32 race = 1; race < MAX_RACES; ++race)
    {
        for (uint32 cls = 1; cls < MAX_CLASSES; ++cls)
        {
            int32 classRaceProb =
                sConfigMgr->GetIntDefault("AiPlayerbot.ClassRaceProb." + std::to_string(cls) + "." + std::to_string(race), -1);
            if (classRaceProb >= 0)
                classRaceProbability[cls][race] = classRaceProb > 0 ? static_cast<uint32>(classRaceProb) : 0;

            if (!sObjectMgr->GetPlayerInfo(race, cls))
                classRaceProbability[cls][race] = 0;

            classRaceProbabilityTotal += classRaceProbability[cls][race];
        }
    }
    botTextLocaleWeightsEnabled = false;
    botTextLocaleWeights.fill(0);
    std::string botTextLocaleWeightsStr = sConfigMgr->GetStringDefault("AiPlayerbot.BotTextLocaleWeights", "");
    if (!botTextLocaleWeightsStr.empty())
    {
        auto Trim = [](std::string& value)
        {
            size_t start = 0;
            while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])))
                ++start;

            size_t end = value.size();
            while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])))
                --end;

            value = value.substr(start, end - start);
        };

        uint32 totalWeight = 0;
        std::vector<std::string> entries = split(botTextLocaleWeightsStr, ',');
        for (std::string& entry : entries)
        {
            Trim(entry);
            if (entry.empty())
                continue;

            size_t sep = entry.find(':');
            if (sep == std::string::npos)
                sep = entry.find('=');
            if (sep == std::string::npos)
            {
                TC_LOG_WARN("playerbots", "Ignoring bot text locale weight '%s' (expected locale:weight)", entry.c_str());
                continue;
            }

            std::string localeName = entry.substr(0, sep);
            std::string weightStr = entry.substr(sep + 1);
            Trim(localeName);
            Trim(weightStr);

            if (localeName.empty() || weightStr.empty())
                continue;

            bool localeFound = false;
            LocaleConstant locale = LOCALE_enUS;
            for (uint32 i = 0; i < TOTAL_LOCALES; ++i)
            {
                if (localeName == localeNames[i])
                {
                    locale = static_cast<LocaleConstant>(i);
                    localeFound = true;
                    break;
                }
            }

            if (!localeFound)
            {
                TC_LOG_WARN("playerbots", "Unknown bot text locale '%s' in AiPlayerbot.BotTextLocaleWeights", localeName.c_str());
                continue;
            }

            if (locale >= MAX_LOCALES)
            {
                TC_LOG_WARN("playerbots", "Ignoring bot text locale '%s' because it exceeds MAX_LOCALES (%u)",
                    localeName.c_str(), MAX_LOCALES - 1);
                continue;
            }

            uint32 weight = atoi(weightStr.c_str());
            botTextLocaleWeights[locale] = weight;
            totalWeight += weight;
        }

        if (totalWeight > 0)
        {
            botTextLocaleWeightsEnabled = true;
            TC_LOG_INFO("playerbots", "Bot text locale weights enabled (total=%u)", totalWeight);
        }
    }

    botActiveAlone = sConfigMgr->GetIntDefault("AiPlayerbot.BotActiveAlone", 10);
    BotActiveAloneForceWhenInRadius = sConfigMgr->GetIntDefault("AiPlayerbot.BotActiveAloneForceWhenInRadius", 150);
    BotActiveAloneForceWhenInZone = sConfigMgr->GetBoolDefault("AiPlayerbot.BotActiveAloneForceWhenInZone", true);
    BotActiveAloneForceWhenInMap = sConfigMgr->GetBoolDefault("AiPlayerbot.BotActiveAloneForceWhenInMap", false);
    BotActiveAloneForceWhenIsFriend = sConfigMgr->GetBoolDefault("AiPlayerbot.BotActiveAloneForceWhenIsFriend", true);
    BotActiveAloneForceWhenInGuild = sConfigMgr->GetBoolDefault("AiPlayerbot.BotActiveAloneForceWhenInGuild", true);
    diffWithPlayer = sConfigMgr->GetIntDefault("AiPlayerbot.DiffWithPlayer", 100);
    diffEmpty = sConfigMgr->GetIntDefault("AiPlayerbot.DiffEmpty", 200);
    disableBotOptimizations = sConfigMgr->GetBoolDefault("AiPlayerbot.DisableBotOptimizations", false);
    disableActivityPriorities = sConfigMgr->GetBoolDefault("AiPlayerbot.DisableActivityPriorities", false);
    botActiveAloneSmartScale = sConfigMgr->GetBoolDefault("AiPlayerbot.botActiveAloneSmartScale", true);
    botActiveAloneSmartScaleDiffLimitfloor =
        sConfigMgr->GetIntDefault("AiPlayerbot.botActiveAloneSmartScaleDiffLimitfloor", 50);
    botActiveAloneSmartScaleDiffLimitCeiling =
        sConfigMgr->GetIntDefault("AiPlayerbot.botActiveAloneSmartScaleDiffLimitCeiling", 200);
    botActiveAloneSmartScaleWhenMinLevel =
        sConfigMgr->GetIntDefault("AiPlayerbot.botActiveAloneSmartScaleWhenMinLevel", 1);
    botActiveAloneSmartScaleWhenMaxLevel =
        sConfigMgr->GetIntDefault("AiPlayerbot.botActiveAloneSmartScaleWhenMaxLevel", 80);
    respawnModNeutral = sConfigMgr->GetFloatDefault("AiPlayerbot.RespawnModNeutral", 10.0f);
    respawnModHostile = sConfigMgr->GetFloatDefault("AiPlayerbot.RespawnModHostile", 5.0f);
    respawnModThreshold = sConfigMgr->GetIntDefault("AiPlayerbot.RespawnModThreshold", 10);
    respawnModMax = sConfigMgr->GetIntDefault("AiPlayerbot.RespawnModMax", 18);
    respawnModForPlayerBots = sConfigMgr->GetBoolDefault("AiPlayerbot.RespawnModForPlayerBots", false);
    respawnModForInstances = sConfigMgr->GetBoolDefault("AiPlayerbot.RespawnModForInstances", false);

    randomBotAutoJoinArenaBracket = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAutoJoinArenaBracket", 7);
    randomBotAutoJoinICBrackets = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotAutoJoinICBrackets", "0,1");
    randomBotAutoJoinEYBrackets = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotAutoJoinEYBrackets", "0,1,2");
    randomBotAutoJoinAVBrackets = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotAutoJoinAVBrackets", "0,1,2,3");
    randomBotAutoJoinABBrackets = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotAutoJoinABBrackets", "0,1,2,3,4,5,6");
    randomBotAutoJoinWSBrackets = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotAutoJoinWSBrackets", "0,1,2,3,4,5,6,7");

    randomBotAutoJoinBGICCount = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAutoJoinBGICCount", 0);
    randomBotAutoJoinBGEYCount = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAutoJoinBGEYCount", 0);
    randomBotAutoJoinBGAVCount = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAutoJoinBGAVCount", 0);
    randomBotAutoJoinBGABCount = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAutoJoinBGABCount", 0);
    randomBotAutoJoinBGWSCount = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAutoJoinBGWSCount", 0);
    randomBotAutoJoinBGRatedArena2v2Count = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAutoJoinBGRatedArena2v2Count", 0);
    randomBotAutoJoinBGRatedArena3v3Count = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAutoJoinBGRatedArena3v3Count", 0);
    randomBotAutoJoinBGRatedArena5v5Count = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAutoJoinBGRatedArena5v5Count", 0);
    maxAddedBotsPerClass = sConfigMgr->GetIntDefault("AiPlayerbot.MaxAddedBotsPerClass", 50);
    maxAddedBots = sConfigMgr->GetIntDefault("AiPlayerbot.MaxAddedBots", 200);
    minRandomBots = sConfigMgr->GetIntDefault("AiPlayerbot.MinRandomBots", 50);
    maxRandomBots = sConfigMgr->GetIntDefault("AiPlayerbot.MaxRandomBots", 200);
    disableRandomLevels = sConfigMgr->GetBoolDefault("AiPlayerbot.DisableRandomLevels", false);
    randomBotMaxLevel = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotMaxLevel", 90);
    randomBotMinLevel = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotMinLevel", 1);
    syncLevelWithPlayers = sConfigMgr->GetBoolDefault("AiPlayerbot.SyncLevelWithPlayers", false);
    randombotStartingLevel = sConfigMgr->GetIntDefault("AiPlayerbot.RandombotStartingLevel", 5);
    randomBotPreQuests = sConfigMgr->GetBoolDefault("AiPlayerbot.PreQuests", false);
    randomBotQuestIds.clear();
    LoadList<std::vector<uint32>>(sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotQuestIds",
        "7848,3802,5505,6502,7761,10277,10285,11492,24499,24511,24710,24712"), randomBotQuestIds);
    syncLevelMaxAbove = sConfigMgr->GetIntDefault("AiPlayerbot.SyncLevelMaxAbove", 5);
    syncLevelNoPlayer = sConfigMgr->GetIntDefault("AiPlayerbot.SyncLevelNoPlayer", randombotStartingLevel);
    if (randomBotMaxLevel > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        randomBotMaxLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    if (randomBotMinLevel > randomBotMaxLevel)
        randomBotMinLevel = randomBotMaxLevel;
    randomBotMaxLevelChance = sConfigMgr->GetFloatDefault("AiPlayerbot.RandomBotMaxLevelChance", 0.25f);
    randomBotsPerInterval = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotsPerInterval", 60);
    randomBotsMaxLoginsPerInterval = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotsMaxLoginsPerInterval", 10);
    randomBotsDatabasePingInterval = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotsDatabasePingInterval", 1000);
    randomBotsDatabaseDelayThreshold = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotsDatabaseDelayThreshold", 10000);
    randomBotUpdateInterval = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotUpdateInterval", 1000);
    randomBotCountChangeMinInterval =
        sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotCountChangeMinInterval", 30 * MINUTE);
    randomBotCountChangeMaxInterval =
        sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotCountChangeMaxInterval", 2 * HOUR);
    minRandomBotInWorldTime = sConfigMgr->GetIntDefault("AiPlayerbot.MinRandomBotInWorldTime", 2 * HOUR);
    maxRandomBotInWorldTime = sConfigMgr->GetIntDefault("AiPlayerbot.MaxRandomBotInWorldTime", 14 * 24 * HOUR);
    minRandomBotRandomizeTime = sConfigMgr->GetIntDefault("AiPlayerbot.MinRandomBotRandomizeTime", 2 * HOUR);
    maxRandomBotRandomizeTime = sConfigMgr->GetIntDefault("AiPlayerbot.MaxRandomBotRandomizeTime", 14 * 24 * HOUR);
    minRandomBotTeleportInterval = sConfigMgr->GetIntDefault("AiPlayerbot.MinRandomBotTeleportInterval", 1 * HOUR);
    maxRandomBotTeleportInterval = sConfigMgr->GetIntDefault("AiPlayerbot.MaxRandomBotTeleportInterval", 5 * HOUR);
    randomBotTeleportNearPlayer = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotTeleportNearPlayer", true);
    randomBotTeleportNearPlayerMaxAmount = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotTeleportNearPlayerMaxAmount", 8);
    randomBotTeleportNearPlayerMaxAmountRadius = sConfigMgr->GetFloatDefault("AiPlayerbot.RandomBotTeleportNearPlayerMaxAmountRadius", 120.0f);
    minRandomBotReviveTime = sConfigMgr->GetIntDefault("AiPlayerbot.MinRandomBotReviveTime", 60);
    maxRandomBotReviveTime = sConfigMgr->GetIntDefault("AiPlayerbot.MaxRandomBotReviveTime", 300);

    randomBotLoginAtStartup = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotLoginAtStartup", true);
    openGoSpell = sConfigMgr->GetIntDefault("AiPlayerbot.OpenGoSpell", 6477);

    randomChangeMultiplier = sConfigMgr->GetFloatDefault("AiPlayerbot.RandomChangeMultiplier", 1.0);

    randomBotCombatStrategies = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotCombatStrategies", "-threat");
    randomBotNonCombatStrategies = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotNonCombatStrategies", "");
    combatStrategies = sConfigMgr->GetStringDefault("AiPlayerbot.CombatStrategies", "+custom::say");
    nonCombatStrategies = sConfigMgr->GetStringDefault("AiPlayerbot.NonCombatStrategies", "+custom::say,+return");


    RandomPlayerbotFactory::CreateRandomBots();
    if (World::IsStopped())
    {
        return true;
    }

    sRandomPlayerbotMgr->PrepareAddclassCache();
    sRandomPlayerbotMgr->PrepareTeleportCache();

    sPlayerbotTextMgr->LoadBotTexts();
    sPlayerbotTextMgr->LoadBotTextChance();
    sPlayerbotHelpMgr->LoadBotHelpTexts();

    TC_LOG_INFO("server.loading", "---------------------------------------");
    TC_LOG_INFO("server.loading", "          Loading TalentSpecs          ");
    TC_LOG_INFO("server.loading", "---------------------------------------");
    for (uint32 cls = 1; cls < MAX_CLASSES; ++cls)
    {
        for (uint32 spec = 0; spec < MAX_SPECIALIZATIONS; ++spec)
        {
            std::ostringstream os;
            os << "AiPlayerbot.PremadeSpecName." << cls << "." << spec;
            std::string loadSpec = sConfigMgr->GetStringDefault(os.str().c_str(), "", false);
            if (!loadSpec.empty())
            {
                std::string premadeSpecLinkLoader[MAX_CLASSES][MAX_SPECIALIZATIONS];
                premadeSpecName[cls][spec] = loadSpec;

                os.str("");
                os.clear();
                os << "AiPlayerbot.PremadeSpecLink." << cls << "." << spec;

                premadeSpecLinkLoader[cls][spec] = sConfigMgr->GetStringDefault(os.str().c_str(), "", true);
                std::vector<std::string> splitSpec = split(premadeSpecLinkLoader[cls][spec], ',');
                for (std::string& split : splitSpec)
                {
                    if (split.size() != 0)
                    {
                        premadeSpecLink[cls][spec].push_back(atoi(split.c_str()));
                    }
                }

                TC_LOG_DEBUG("playerbots", "Premade spec loaded: %s - %s", loadSpec.c_str(), premadeSpecLinkLoader[cls][spec].c_str());
            }
            os.str("");
            os.clear();
        }
    }


    TC_LOG_INFO("server.loading", "---------------------------------------");
    TC_LOG_INFO("server.loading", "        AI Playerbots initialized       ");
    TC_LOG_INFO("server.loading", "---------------------------------------");

    return true;
}

bool PlayerbotAIConfig::IsPvpProhibited(uint32 zoneId, uint32 areaId)
{
    return IsInPvpProhibitedZone(zoneId) || IsInPvpProhibitedArea(areaId) || IsInPvpProhibitedZone(areaId);
}

bool PlayerbotAIConfig::IsInPvpProhibitedZone(uint32 id)
{
    return std::find(pvpProhibitedZoneIds.begin(), pvpProhibitedZoneIds.end(), id) != pvpProhibitedZoneIds.end();
}

bool PlayerbotAIConfig::IsInPvpProhibitedArea(uint32 id)
{
    return std::find(pvpProhibitedAreaIds.begin(), pvpProhibitedAreaIds.end(), id) != pvpProhibitedAreaIds.end();
}

std::string const PlayerbotAIConfig::GetTimestampStr()
{
    time_t t = time(nullptr);
    tm* aTm = localtime(&t);
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    char buf[20];
    snprintf(buf, 20, "%04d-%02d-%02d %02d-%02d-%02d", aTm->tm_year + 1900, aTm->tm_mon + 1, aTm->tm_mday, aTm->tm_hour,
        aTm->tm_min, aTm->tm_sec);
    return std::string(buf);
}

bool PlayerbotAIConfig::openLog(std::string const fileName, char const* mode)
{
    if (!hasLog(fileName))
        return false;

    auto logFileIt = logFiles.find(fileName);
    if (logFileIt == logFiles.end())
    {
        logFiles.insert(std::make_pair(fileName, std::make_pair(nullptr, false)));
        logFileIt = logFiles.find(fileName);
    }

    FILE* file = logFileIt->second.first;
    bool fileOpen = logFileIt->second.second;

    if (fileOpen)  // close log file
        fclose(file);

    std::string m_logsDir = sConfigMgr->GetStringDefault("LogsDir", "");
    if (!m_logsDir.empty())
    {
        if ((m_logsDir.at(m_logsDir.length() - 1) != '/') && (m_logsDir.at(m_logsDir.length() - 1) != '\\'))
            m_logsDir.append("/");
    }

    file = fopen((m_logsDir + fileName).c_str(), mode);
    fileOpen = true;

    logFileIt->second.first = file;
    logFileIt->second.second = fileOpen;

    return true;
}

void PlayerbotAIConfig::log(std::string const fileName, char const* str, ...)
{
    if (!str)
        return;

    std::lock_guard<std::mutex> guard(m_logMtx);

    if (!isLogOpen(fileName) && !openLog(fileName, "a"))
        return;

    FILE* file = logFiles.find(fileName)->second.first;

    va_list ap;
    va_start(ap, str);
    vfprintf(file, str, ap);
    fprintf(file, "\n");
    va_end(ap);
    fflush(file);

    fflush(stdout);
}

bool PlayerbotAIConfig::IsInRandomAccountList(uint32 id)
{
    return std::find(randomBotAccounts.begin(), randomBotAccounts.end(), id) != randomBotAccounts.end();
}
