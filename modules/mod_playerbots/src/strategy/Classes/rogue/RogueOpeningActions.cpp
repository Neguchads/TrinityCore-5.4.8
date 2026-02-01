

#include "RogueOpeningActions.h"

#include "Playerbots.h"

Value<Unit*>* CastSapAction::GetTargetValue() { return context->GetValue<Unit*>("cc target", getName()); }
