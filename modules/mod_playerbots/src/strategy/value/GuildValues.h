#ifndef _PLAYERBOT_GUILDVALUES_H
#define _PLAYERBOT_GUILDVALUES_H

#include "Value.h"

class PlayerbotAI;

class PetitionSignsValue : public Uint8CalculatedValue
{
public:
    PetitionSignsValue(PlayerbotAI* botAI) : Uint8CalculatedValue(botAI, "petition signs", 5) {}

    uint8 Calculate() override;
};

class CanHandInPetitionValue : public BoolCalculatedValue
{
public:
    CanHandInPetitionValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "can hand in petition", 5) {}

    bool Calculate() override;
};

class CanBuyTabardValue : public BoolCalculatedValue
{
public:
    CanBuyTabardValue(PlayerbotAI* botAI) : BoolCalculatedValue(botAI, "can buy tabard", 5) {}

    bool Calculate() override;
};

#endif

