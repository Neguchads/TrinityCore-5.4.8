

#include "HunterTriggers.h"

#include "GenericTriggers.h"
#include "HunterActions.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "ServerFacade.h"
#include "SharedDefines.h"

bool HunterFocusFireTrigger::IsActive()
{
    Unit* pet = AI_VALUE(Unit*, "pet target");
    if (!pet)
        return false;

    Aura* aura = botAI->GetAura("frenzy", pet, false, true, 5);
    return aura != nullptr;
}

bool HunterAspectOfTheHawkTrigger::IsActive()
{
    Unit* target = GetTarget();
    return SpellTrigger::IsActive() &&
        !botAI->HasAnyAuraOf(target, "aspect of the hawk", "aspect of the dragonhawk", "aspect of the iron hawk", nullptr) &&
        (!AI_VALUE2(bool, "has mana", "self target") || AI_VALUE2(uint8, "mana", "self target") > 70);
}

bool HunterAspectOfTheViperTrigger::IsActive()
{
    return SpellTrigger::IsActive() && !botAI->HasAura(spell, GetTarget()) &&
        AI_VALUE2(uint8, "mana", "self target") < (sPlayerbotAIConfig->lowMana / 2);
}

bool HunterAspectOfThePackTrigger::IsActive()
{
    return BuffTrigger::IsActive() && !botAI->HasAura("aspect of the cheetah", GetTarget());
}

bool NoTrackTrigger::IsActive()
{
    std::vector<std::string> trackList = {
        "track beasts",
        "track demons",
        "track dragonkin",
        "track elementals",
        "track giants",
        "track hidden",
        "track humanoids"
    };

    for (std::string const& track : trackList)
    {
        if (botAI->HasAura(track, bot))
            return false;
    }

    return true;
}
