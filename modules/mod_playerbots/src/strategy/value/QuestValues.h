#ifndef _PLAYERBOT_QUESTVALUES_H
#define _PLAYERBOT_QUESTVALUES_H

#include "NamedObjectContext.h"
#include "TravelMgr.h"
#include "Value.h"

class Player;
class PlayerbotAI;

struct CreatureData;
struct GameObjectData;

enum class QuestRelationFlag : uint32
{
    none = 0,
    objective1 = 1,
    objective2 = 2,
    objective3 = 4,
    objective4 = 8,
    questGiver = 16,
    questTaker = 32,
    maxQuestRelationFlag = 64
};

// Local dialog status values used by playerbots (not provided by TC core).
enum DialogStatus : uint32
{
    DIALOG_STATUS_NONE = 0,
    DIALOG_STATUS_UNAVAILABLE = 1,
    DIALOG_STATUS_AVAILABLE = 2,
    DIALOG_STATUS_LOW_LEVEL_AVAILABLE = 3,
    DIALOG_STATUS_INCOMPLETE = 4,
    DIALOG_STATUS_REWARD = 5,
    DIALOG_STATUS_REWARD_REP = 6,
    DIALOG_STATUS_REWARD2 = 7
};

typedef std::unordered_map<uint32, uint32> questRelationMap;
typedef std::unordered_map<int32, questRelationMap> entryQuestRelationMap;
typedef std::unordered_map<int32, std::vector<GuidPosition>> questEntryGuidps;
typedef std::unordered_map<uint32, questEntryGuidps> questRelationGuidps;
typedef std::unordered_map<uint32, questRelationGuidps> questGuidpMap;
typedef std::unordered_map<uint32, std::vector<GuidPosition>> questGiverMap;

class EntryQuestRelationMapValue : public SingleCalculatedValue<entryQuestRelationMap>
{
public:
    EntryQuestRelationMapValue(PlayerbotAI* botAI) : SingleCalculatedValue(botAI, "entry quest relation map") {}

    entryQuestRelationMap Calculate() override;
};

class FindQuestObjectData
{
public:
    FindQuestObjectData() { GetObjectiveEntries(); }

    void GetObjectiveEntries();
    void operator()(CreatureData const& creatureData);
    void operator()(GameObjectData const& gameobjectData);
    questGuidpMap GetResult() const { return data; }

private:
    entryQuestRelationMap relationMap;
    questGuidpMap data;
};

class QuestGuidpMapValue : public SingleCalculatedValue<questGuidpMap>
{
public:
    QuestGuidpMapValue(PlayerbotAI* botAI) : SingleCalculatedValue(botAI, "quest guidp map") {}

    questGuidpMap Calculate() override;
};

class QuestGiversValue : public SingleCalculatedValue<questGiverMap>, public Qualified
{
public:
    QuestGiversValue(PlayerbotAI* botAI) : SingleCalculatedValue(botAI, "quest givers") {}

    questGiverMap Calculate() override;
};

class ActiveQuestGiversValue : public CalculatedValue<std::vector<GuidPosition>>
{
public:
    ActiveQuestGiversValue(PlayerbotAI* botAI) : CalculatedValue(botAI, "active quest givers", 5) {}

    std::vector<GuidPosition> Calculate() override;
};

class ActiveQuestTakersValue : public CalculatedValue<std::vector<GuidPosition>>
{
public:
    ActiveQuestTakersValue(PlayerbotAI* botAI) : CalculatedValue(botAI, "active quest takers", 5) {}

    std::vector<GuidPosition> Calculate() override;
};

class ActiveQuestObjectivesValue : public CalculatedValue<std::vector<GuidPosition>>
{
public:
    ActiveQuestObjectivesValue(PlayerbotAI* botAI) : CalculatedValue(botAI, "active quest objectives", 5) {}

    std::vector<GuidPosition> Calculate() override;
};

class FreeQuestLogSlotValue : public Uint8CalculatedValue
{
public:
    FreeQuestLogSlotValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "free quest log slots", 2) {}

    uint8 Calculate() override;
};

class DialogStatusValue : public Uint32CalculatedValue, public Qualified
{
public:
    DialogStatusValue(PlayerbotAI* botAI, std::string const name = "dialog status")
        : Uint32CalculatedValue(botAI, name, 2)
    {
    }

    static uint32 getDialogStatus(Player* bot, int32 questgiver, uint32 questId = 0);

    uint32 Calculate() override;
};

class DialogStatusQuestValue : public DialogStatusValue
{
public:
    DialogStatusQuestValue(PlayerbotAI* botAI) : DialogStatusValue(botAI, "dialog status quest") {}

    uint32 Calculate() override;
};

class CanAcceptQuestValue : public BoolCalculatedValue, public Qualified
{
public:
    CanAcceptQuestValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "can accept quest npc") {}

    bool Calculate() override;
};

class CanAcceptQuestLowLevelValue : public BoolCalculatedValue, public Qualified
{
public:
    CanAcceptQuestLowLevelValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "can accept quest low level npc") {}

    bool Calculate() override;
};

class CanTurnInQuestValue : public BoolCalculatedValue, public Qualified
{
public:
    CanTurnInQuestValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "can turn in quest npc") {}

    bool Calculate() override;
};

#endif
