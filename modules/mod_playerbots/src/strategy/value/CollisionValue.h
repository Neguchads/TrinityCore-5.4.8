#ifndef _PLAYERBOT_COLLISIONVALUE_H
#define _PLAYERBOT_COLLISIONVALUE_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;

class CollisionValue : public BoolCalculatedValue, public Qualified
{
public:
    CollisionValue(PlayerbotAI* botAI, std::string const name = "collision")
        : BoolCalculatedValue(botAI, name), Qualified()
    {
    }

    bool Calculate() override;
};

#endif
