

#ifndef _PLAYERBOT_PALADINAIOBJECTCONTEXT_H
#define _PLAYERBOT_PALADINAIOBJECTCONTEXT_H

#include "AiObjectContext.h"

class PlayerbotAI;

class PaladinAiObjectContext : public AiObjectContext
{
public:
    PaladinAiObjectContext(PlayerbotAI* botAI);
};

#endif
