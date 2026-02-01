

#include "RogueComboActions.h"

#include "Playerbots.h"

bool CastComboAction::isUseful()
{
    return CastMeleeSpellAction::isUseful() && AI_VALUE2(uint8, "combo", "current target") < 5;
}
