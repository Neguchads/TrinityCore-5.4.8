#ifndef _PLAYERBOT_RTITARGETVALUE_H
#define _PLAYERBOT_RTITARGETVALUE_H

#include "TargetValue.h"

class PlayerbotAI;
class Unit;

class RtiTargetValue : public TargetValue
{
public:
    RtiTargetValue(PlayerbotAI* botAI, std::string const type = "rti", std::string const name = "rti target")
        : TargetValue(botAI, name), type(type)
    {
    }

    static constexpr int32 starIndex = 0;
    static constexpr int32 circleIndex = 1;
    static constexpr int32 diamondIndex = 2;
    static constexpr int32 triangleIndex = 3;
    static constexpr int32 moonIndex = 4;
    static constexpr int32 squareIndex = 5;
    static constexpr int32 crossIndex = 6;
    static constexpr int32 skullIndex = 7;

    static int32 GetRtiIndex(std::string const rti);
    Unit* Calculate() override;

private:
    std::string const type;
};

class RtiCcTargetValue : public RtiTargetValue
{
public:
    RtiCcTargetValue(PlayerbotAI* botAI, std::string const name = "rti cc target")
        : RtiTargetValue(botAI, "rti cc", name)
    {
    }
};

#endif
