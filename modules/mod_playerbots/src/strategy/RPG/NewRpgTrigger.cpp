#include "NewRpgTriggers.h"
#include "PlayerbotAI.h"

bool NewRpgStatusTrigger::IsActive()
{
    return botAI->GetRpgInfo().status == status;
}
