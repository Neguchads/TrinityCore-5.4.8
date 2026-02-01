

#ifndef _PLAYERBOT_HUNTERAIOBJECTCONTEXT_H
#define _PLAYERBOT_HUNTERAIOBJECTCONTEXT_H

#include "AiObjectContext.h"

class PlayerbotAI;

class HunterAiObjectContext : public AiObjectContext
{
public:
    HunterAiObjectContext(PlayerbotAI* botAI);
};

#endif
