#ifndef _PLAYERBOT_PLAYERBOTAI_H
#define _PLAYERBOT_PLAYERBOTAI_H

#include <functional>
#include <future>
#include <map>
#include <mutex>
#include <queue>
#include <stack>
#include <vector>

#include "Chat.h"
#include "Event.h"
#include "Item.h"
#include "PlayerbotAIBase.h"
#include "PlayerbotAIConfig.h"
#include "NewRpgStrategy.h"
#include "WorldPacket.h"
#include "PlayerbotSecurity.h"

enum BotState
{
    BOT_STATE_COMBAT = 0,
    BOT_STATE_NON_COMBAT = 1,
    BOT_STATE_DEAD = 2,

    BOT_STATE_MAX
};

enum BotRoles : uint8
{
    BOT_ROLE_NONE = 0x00,
    BOT_ROLE_TANK = 0x01,
    BOT_ROLE_HEALER = 0x02,
    BOT_ROLE_DPS = 0x04
};

enum ActivityType
{
    GRIND_ACTIVITY = 1,
    RPG_ACTIVITY = 2,
    TRAVEL_ACTIVITY = 3,
    OUT_OF_PARTY_ACTIVITY = 4,
    PACKET_ACTIVITY = 5,
    DETAILED_MOVE_ACTIVITY = 6,
    PARTY_ACTIVITY = 7,
    ALL_ACTIVITY = 8,

    MAX_ACTIVITY_TYPE
};

// CMaNGOS parity: deterministic per-bot "personality" numbers (seeded by purpose).
enum class BotTypeNumber : uint8
{
    GUILDER_TYPE_NUMBER = 3
};

// CMaNGOS parity: guild-size preference profile.
enum class GuilderType : uint8
{
    SOLO = 0,
    TINY = 30,
    SMALL = 50,
    MEDIUM = 70,
    LARGE = 120,
    MASSIVE = 250
};

class Aura;
class AiObjectContext;
class Creature;
class Engine;
class ExternalEventHelper;
class GameObject;
class ObjectGuid;
class Player;
class Position;
class Spell;
class Unit;
class Item;
class WorldSession;
class WorldObject;
struct CreatureData;

class PlayerbotChatHandler : protected ChatHandler
{
public:
    explicit PlayerbotChatHandler(Player* pMasterPlayer);
    void sysmessage(std::string const str) { SendSysMessage(str.c_str()); }
    uint32 extractQuestId(std::string const str);
    uint32 extractSpellId(std::string const str)
    {
        char* source = (char*)str.c_str();
        return extractSpellIdFromLink(source);
    }
};

class MinValueCalculator
{
public:
    MinValueCalculator(float def = 0.0f) : param(nullptr), minValue(def) {}

    void probe(float value, void* p)
    {
        if (!param || minValue >= value)
        {
            minValue = value;
            param = p;
        }
    }

    void* param;
    float minValue;
};

class PacketHandlingHelper
{
public:
    void AddHandler(uint16 opcode, std::string const handler);
    void Handle(ExternalEventHelper& helper);
    void AddPacket(WorldPacket const& packet);

private:
    std::map<uint16, std::string> _handlers;
    std::stack<WorldPacket> _queue;
    std::mutex _mutex;
};

class PlayerbotAI : public PlayerbotAIBase
{
public:
    using futurePackets = std::future<std::vector<std::pair<WorldPacket, uint32>>>;

    PlayerbotAI();
    PlayerbotAI(Player* bot);
    virtual ~PlayerbotAI();

    /* static */
    static uint32 GetMixedGearScore(Player* player, bool withBags, bool withBank, uint32 topN = 0);
    static float GetItemScoreMultiplier(ItemQualities quality);
private:
    static void _fillGearScoreData(Player* player, Item* item, std::vector<uint32>* gearScore, uint32& twoHandScore, bool mixed = false);

public:
    void UpdateAI(uint32 elapsed, bool minimal = false) override;
    void UpdateAIInternal(uint32 elapsed, bool minimal = false) override;
    bool AllowActivity(ActivityType activityType = ALL_ACTIVITY, bool checkNow = false);
    bool AllowActive(ActivityType activityType);
    void DoNextAction(bool min);

    void HandleBotOutgoingPacket(WorldPacket const& packet);
    void HandleMasterIncomingPacket(WorldPacket const& packet);
    void HandleMasterOutgoingPacket(WorldPacket const& packet);
    void HandleTeleportAck();
    
    void Reset(bool full = false);
    void ChangeEngine(BotState type);
    void ReInitCurrentEngine();
    virtual bool DoSpecificAction(std::string const name, Event event = Event(), bool silent = false, std::string const qualifier = "");

    void ResetStrategies();
    void ChangeStrategy(std::string const name, BotState type);
    void ClearStrategies(BotState type);
    std::vector<std::string> GetStrategies(BotState type);

    Player* GetBot() { return bot; }
    Player* GetMaster() { return master; }
    Player* GetGroupMaster();
    Player* GetGroupLeader();
    std::vector<Player*> GetPlayersInGroup() const;
    int32 GetGroupSlotIndex(Player* player) const;
    uint32 GetFixedBotNumer(uint32 maxNum = 100, float cyclePerMin = 1);
    uint32 GetFixedBotNumber(BotTypeNumber typeNumber, uint32 maxNum = 100, float cyclePerMin = 1.0f, bool ignoreGuid = false);
    uint32 AutoScaleActivity(uint32 mod);
    bool IsInRealGuild();
    GuilderType GetGuilderType();
    uint32 GetMaxPreferedGuildSize();
    Creature* GetCreature(ObjectGuid guid);
    Unit* GetUnit(ObjectGuid guid);
    Player* GetPlayer(ObjectGuid guid);
    static Unit* GetUnit(CreatureData const* creatureData);
    GameObject* GetGameObject(ObjectGuid guid);
    // static GameObject* GetGameObject(GameObjectData const* gameObjectData);
    WorldObject* GetWorldObject(ObjectGuid guid);
    AiObjectContext* GetAiObjectContext() { return _aiObjectContext; }
    BotState GetState() { return _currentState; };
    NewRpgInfo& GetRpgInfo() { return _rpgInfo; }
    const NewRpgInfo& GetRpgInfo() const { return _rpgInfo; }
    float GetRange(std::string const type);

    bool IsOpposing(Player* player);
    static bool IsOpposing(uint8 race1, uint8 race2);
    // Checks if the bot is really a player. Players always have themselves as master.
    bool IsRealPlayer() { return master ? (master == bot) : false; }
    // Bot has a master that is a player.
    bool HasRealPlayerMaster();
    // Bot has a master that is activly playing.
    bool HasActivePlayerMaster();
    bool HasCheat(BotCheatMask mask)
    {
        return (static_cast<uint32>(mask) & static_cast<uint32>(cheatMask)) != 0 ||
            (static_cast<uint32>(mask) & static_cast<uint32>(sPlayerbotAIConfig->botCheatMask)) != 0;
    }
    BotCheatMask GetCheat() { return cheatMask; }
    void SetCheat(BotCheatMask mask) { cheatMask = mask; }
    bool HasPlayerNearby(WorldPosition* pos, float range = sPlayerbotAIConfig->reactDistance);
    bool HasPlayerNearby(float range = sPlayerbotAIConfig->reactDistance);
    bool HasManyPlayersNearby(uint32 trigerrValue = 20, float range = sPlayerbotAIConfig->sightDistance);

    // Check if player is safe to use.
    bool IsSafe(Player* player);
    bool IsSafe(WorldObject* obj);
    bool HasAggro(Unit* unit);

    // Get the group leader or the master of the bot.
    // Checks if the bot is summoned as alt of a player
    bool IsAlt();
    bool IsInVehicle(bool canControl = false, bool canCast = false, bool canAttack = false, bool canTurn = false, bool fixed = false);
    bool CanCastVehicleSpell(uint32 spellId, Unit* target);
    bool CastVehicleSpell(uint32 spellId, Unit* target);
    bool HasItemInInventory(uint32 itemId);
    std::vector<Item*> GetInventoryItems();
    bool ImbueItem(Item* item, Unit* target);
    void AddTimedEvent(std::function<void()> fn, uint32 delayMs);
    void QueueChatResponse(uint32 msgType, ObjectGuid guid1, ObjectGuid guid2, std::string message,
        std::string chanName, std::string name, bool noDelay = false);
    static void SendDelayedPacket(WorldSession* session, futurePackets futPackets);
    void ReceiveDelayedPacket(futurePackets futPackets);
    void QueuePacket(WorldPacket& pkt);
    void StopMoving();
    void Unmount();
    void AccelerateRespawn(Creature* creature, float accelMod = 0.0f);
    void AccelerateRespawn(ObjectGuid guid, float accelMod = 0.0f)
    {
        Creature* creature = GetCreature(guid);
        if (creature)
            AccelerateRespawn(creature, accelMod);
    }

    bool HasStrategy(std::string const name, BotState type);
    bool ContainsStrategy(StrategyType type);

    void SetMaster(Player* newMaster) { master = newMaster; }

    // Role helpers (wrappers for PlayerBotSpec)
    static bool IsTank(Player* player, bool bySpec = false);
    static bool IsHeal(Player* player, bool bySpec = false);
    static bool IsDps(Player* player, bool bySpec = false);
    static bool IsRanged(Player* player, bool bySpec = false);
    static bool IsMelee(Player* player, bool bySpec = false);
    static bool IsRangedDps(Player* player, bool bySpec = false);
    static bool IsMainTank(Player* player);
    static bool IsAssistTank(Player* player);
    static bool IsAssistTankOfIndex(Player* bot, Player* player, int index);
    static bool IsAssistHealOfIndex(Player* bot, Player* player, int index);

    bool IsTank(bool bySpec = false) { return IsTank(bot, bySpec); }
    bool IsHeal(bool bySpec = false) { return IsHeal(bot, bySpec); }
    bool IsDps(bool bySpec = false) { return IsDps(bot, bySpec); }
    bool IsRanged(bool bySpec = false) { return IsRanged(bot, bySpec); }
    bool IsMelee(bool bySpec = false) { return IsMelee(bot, bySpec); }
    bool IsRangedDps(bool bySpec = false) { return IsRangedDps(bot, bySpec); }
    bool IsMainTank() { return IsMainTank(bot); }
    bool IsAssistTank() { return IsAssistTank(bot); }
    bool IsBotMainTank(Player* player) { return IsMainTank(player); }
    bool IsAssistTankOfIndex(Player* player, int index, bool bySpec = false)
    {
        (void)bySpec;
        return PlayerbotAI::IsAssistTankOfIndex(bot, player, index);
    }
    bool IsAssistHealOfIndex(Player* player, int index, bool bySpec = false)
    {
        (void)bySpec;
        return PlayerbotAI::IsAssistHealOfIndex(bot, player, index);
    }

    bool HasSkill(uint32 skillId);

    bool CanMove();

    uint32 GetReactDelay();
    void InterruptSpell();
    virtual bool IsInterruptableSpellCasting(Unit* player, std::string const spell);
    bool canDispel(SpellInfo const* spellInfo, uint32 dispelType);
    virtual bool HasAuraToDispel(Unit* player, uint32 dispelType);
    void RemoveAura(std::string const name);
    void RemoveShapeshift();
    void WaitForSpellCast(Spell* spell);
    void SpellInterrupted(uint32 spellid);
    virtual bool CanCastSpell(std::string const name, Unit* target, Item* itemTarget = nullptr);
    virtual bool CastSpell(std::string const name, Unit* target, Item* itemTarget = nullptr);
    bool CastSpell(uint32 spellId, Unit* target, Item* itemTarget = nullptr);
    bool CastSpell(uint32 spellId, float x, float y, float z, Item* itemTarget = nullptr);
    virtual bool HasAura(std::string const spellName, Unit* player, bool maxStack = false, bool checkIsOwner = false,
        int maxAmount = -1, bool checkDuration = false);
    Aura* GetAura(std::string const spellName, Unit* unit, bool checkIsOwner = false, bool checkDuration = false,
        int checkStack = -1);
    virtual bool HasAnyAuraOf(Unit* player, ...);
    bool CanCastSpell(uint32 spellid, Unit* target, bool checkHasSpell = true, Item* itemTarget = nullptr,
        Item* castItem = nullptr);
    bool CanCastSpell(uint32 spellid, GameObject* goTarget, uint8 effectMask, bool checkHasSpell = true);
    bool CanCastSpell(uint32 spellid, float x, float y, float z, uint8 effectMask, bool checkHasSpell = true,
        Item* itemTarget = nullptr);
    bool HasAura(uint32 spellId, Unit const* player);

    int32 GetNearGroupMemberCount(float dis = sPlayerbotAIConfig->sightDistance);

    InventoryResult CanEquipItem(uint8 slot, uint16& dest, Item* pItem, bool swap, bool not_loading = true) const;
    uint8 FindEquipSlot(ItemTemplate const* proto, uint32 slot, bool swap) const;

    bool TellMaster(std::ostringstream& stream);
    bool TellMaster(std::string const text);
    bool TellMasterNoFacing(std::string const text);
    bool TellMasterNoFacing(std::ostringstream& stream);
    bool Whisper(const std::string& msg, const std::string& receiverName);
    bool Say(const std::string& msg);
    bool Yell(const std::string& msg);
    bool SayToRaid(const std::string& msg);
    bool SayToParty(const std::string& msg);
    bool TellError(std::string const text);
    // Helper for ported actions that need to reply to a specific requester (not always the master).
    bool TellError(Player* requester, std::string const& text);

    PlayerbotSecurity* GetSecurity() { return &security; }
private:
    bool _isBotInitializing;

protected:
    Player* bot;
    Player* master;
    uint32 accountId;
    AiObjectContext* _aiObjectContext;
    Engine* _currentEngine;
    Engine* _engines[BOT_STATE_MAX];
    BotState _currentState;
    NewRpgInfo _rpgInfo;
    //ChatHelper chatHelper;
    //std::list<ChatCommandHolder> chatCommands;
    //std::list<ChatQueuedReply> chatReplies;
    PacketHandlingHelper botOutgoingPacketHandlers;
    PacketHandlingHelper masterIncomingPacketHandlers;
    PacketHandlingHelper masterOutgoingPacketHandlers;
    //CompositeChatFilter chatFilter;
    PlayerbotSecurity security;
    //std::map<std::string, time_t> whispers;
    //std::pair<ChatMsg, time_t> currentChat;
    //static std::set<std::string> unsecuredCommands;
    bool _allowActive[MAX_ACTIVITY_TYPE];
    time_t _allowActiveCheckTimer[MAX_ACTIVITY_TYPE];
    //bool inCombat = false;
    BotCheatMask cheatMask = BotCheatMask::none;
    struct TimedEvent
    {
        uint32 dueTimeMs;
        std::function<void()> fn;
    };
    std::vector<TimedEvent> _timedEvents;
    //Position jumpDestination = Position();
    //uint32 nextTransportCheck = 0;
};

#endif
