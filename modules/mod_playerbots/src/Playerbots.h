

// NOTE: this header guard must be unique. `_PLAYERBOT_H` collides with other codebases/headers
// and can cause this header to be skipped, breaking macros like GET_PLAYERBOT_AI.
#ifndef MOD_PLAYERBOTS_PLAYERBOTS_H
#define MOD_PLAYERBOTS_PLAYERBOTS_H

#include "AiObjectContext.h"
#include "Group.h"
#include "Pet.h"
#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotMgr.h"
#include "RandomPlayerbotMgr.h"
#include "SharedValueContext.h"
#include "Spell.h"
#include "SpellMgr.h"

#define CAST_ANGLE_IN_FRONT (2.f * static_cast<float>(M_PI) / 3.f)
#define EMOTE_ANGLE_IN_FRONT (2.f * static_cast<float>(M_PI) / 6.f)

#define GET_PLAYERBOT_AI(object) sPlayerbotsMgr->GetPlayerbotAI(object)
#define GET_PLAYERBOT_MGR(object) sPlayerbotsMgr->GetPlayerbotMgr(object)

#define AI_VALUE(type, name) context->GetValue<type>(name)->Get()
#define AI_VALUE2(type, name, param) context->GetValue<type>(name, param)->Get()

#define AI_VALUE_LAZY(type, name) context->GetValue<type>(name)->LazyGet()
#define AI_VALUE2_LAZY(type, name, param) context->GetValue<type>(name, param)->LazyGet()

#define AI_VALUE_REF(type, name) context->GetValue<type>(name)->RefGet()

#define SET_AI_VALUE(type, name, value) context->GetValue<type>(name)->Set(value)
#define SET_AI_VALUE2(type, name, param, value) context->GetValue<type>(name, param)->Set(value)
#define RESET_AI_VALUE(type, name) context->GetValue<type>(name)->Reset()
#define RESET_AI_VALUE2(type, name, param) context->GetValue<type>(name, param)->Reset()

#define PAI_VALUE(type, name) sPlayerbotsMgr->GetPlayerbotAI(player)->GetAiObjectContext()->GetValue<type>(name)->Get()
#define PAI_VALUE2(type, name, param) \
    sPlayerbotsMgr->GetPlayerbotAI(player)->GetAiObjectContext()->GetValue<type>(name, param)->Get()
#define GAI_VALUE(type, name) sSharedValueContext->getGlobalValue<type>(name)->Get()
#define GAI_VALUE2(type, name, param) sSharedValueContext->getGlobalValue<type>(name, param)->Get()

#endif // MOD_PLAYERBOTS_PLAYERBOTS_H
