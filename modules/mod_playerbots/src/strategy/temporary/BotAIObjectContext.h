

#ifndef _PLAYERBOT_BOTAIOBJECTCONTEXT_H
#define _PLAYERBOT_BOTAIOBJECTCONTEXT_H

#include "AiObjectContext.h"

class PlayerbotAI;

class BotAiObjectContext : public AiObjectContext
{
public:
    BotAiObjectContext(PlayerbotAI* botAI);
};

#endif
