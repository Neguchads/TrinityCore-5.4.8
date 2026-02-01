

#ifndef _PLAYERBOT_ROGUEAIOBJECTCONTEXT_H
#define _PLAYERBOT_ROGUEAIOBJECTCONTEXT_H

#include "AiObjectContext.h"

class PlayerbotAI;

class RogueAiObjectContext : public AiObjectContext
{
public:
    RogueAiObjectContext(PlayerbotAI* botAI);
};

#endif
