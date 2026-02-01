

#ifndef _PLAYERBOT_DKAIOBJECTCONTEXT_H
#define _PLAYERBOT_DKAIOBJECTCONTEXT_H

#include "AiObjectContext.h"

class PlayerbotAI;

class DKAiObjectContext : public AiObjectContext
{
public:
    DKAiObjectContext(PlayerbotAI* botAI);
};

#endif
