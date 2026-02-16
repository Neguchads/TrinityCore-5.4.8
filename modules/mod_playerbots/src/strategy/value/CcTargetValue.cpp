#include "CcTargetValue.h"

#include "Playerbots.h"
#include "PlayerbotSpec.h"
#include "ServerFacade.h"

class FindTargetForCcStrategy : public FindTargetStrategy
{
public:
    FindTargetForCcStrategy(PlayerbotAI* botAI, std::string const spell)
        : FindTargetStrategy(botAI), spell(spell), maxDistance(0.0f)
    {
    }

public:
    void CheckAttacker(Unit* creature, ThreatManager* threatMgr) override
    {
        Player* bot = botAI->GetBot();
        if (!botAI->CanCastSpell(spell, creature))
            return;

        if (*botAI->GetAiObjectContext()->GetValue<Unit*>("rti cc target") == creature)
        {
            result = creature;
            return;
        }

        if (*botAI->GetAiObjectContext()->GetValue<Unit*>("current target") == creature)
            return;

        uint8 health = static_cast<uint8>(creature->GetHealthPct());
        if (health < sPlayerbotAIConfig->mediumHealth)
            return;

        float minDistance = botAI->GetRange("spell");
        Group* group = bot->GetGroup();
        if (!group)
            return;

        uint32 tankCount = 0;
        uint32 dpsCount = 0;
        GetPlayerCount(creature, &tankCount, &dpsCount);
        if (!tankCount || !dpsCount)
        {
            result = creature;
            return;
        }

        Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); ++itr)
        {
            Player* member = ObjectAccessor::FindPlayer(itr->guid);
            if (!member || !member->IsAlive() || member == bot)
                continue;

            if (!PlayerBotSpec::IsTank(member))
                continue;

            float distance = sServerFacade->GetDistance2d(member, creature);
            if (distance < minDistance)
                minDistance = distance;
        }

        if (!result || minDistance > maxDistance)
        {
            result = creature;
            maxDistance = minDistance;
        }
    }

private:
    std::string const spell;
    float maxDistance;
};

Unit* CcTargetValue::Calculate()
{
    FindTargetForCcStrategy strategy(botAI, qualifier);
    if (Unit* target = FindTarget(&strategy))
        return target;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (!currentTarget || !currentTarget->IsPlayer())
        return nullptr;

    Player* bot = botAI->GetBot();
    bool const isPvpContext = bot->InBattleground() || bot->InArena() || bot->duel || bot->IsPvP();
    if (!isPvpContext)
        return nullptr;

    if (!botAI->CanCastSpell(qualifier, currentTarget))
        return nullptr;

    if (AI_VALUE2(Unit*, "current cc target", qualifier))
        return nullptr;

    return currentTarget;
}
