

#include "DruidBearActions.h"

#include "Playerbots.h"

bool CastMaulAction::isUseful()
{
    return CastMeleeSpellAction::isUseful() && AI_VALUE2(uint8, "rage", "self target") >= 45;
}
