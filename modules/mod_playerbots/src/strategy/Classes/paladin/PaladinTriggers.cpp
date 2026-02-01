

#include "PaladinTriggers.h"

#include "PaladinActions.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"

bool SealTrigger::IsActive()
{
    return !botAI->HasAnyAuraOf(bot, "seal of righteousness", "seal of command", "seal of insight", "seal of truth", nullptr);
}

bool HolyPowerTrigger::IsActive()
{
    return bot->GetPower(Powers::POWER_HOLY_POWER) >= minHolyPower;
}
