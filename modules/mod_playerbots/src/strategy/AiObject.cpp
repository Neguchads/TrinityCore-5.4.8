

#include "AiObject.h"
#include "Playerbots.h"

AiObject::AiObject(PlayerbotAI* botAI)
    : PlayerbotAIAware(botAI),
    bot(botAI->GetBot()),
    context(botAI->GetAiObjectContext())
    /*, chat(botAI->GetChatHelper()*/
{
}

Player* AiObject::GetMaster() { return botAI->GetMaster(); }
