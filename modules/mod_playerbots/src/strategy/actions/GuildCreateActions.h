#ifndef _PLAYERBOT_GUILDCREATEACTIONS_H
#define _PLAYERBOT_GUILDCREATEACTIONS_H

#include "Action.h"

class Player;

// Guild charter automation (CMaNGOS parity).
class BuyPetitionAction : public Action
{
public:
    BuyPetitionAction(PlayerbotAI* botAI, std::string const& name = "buy petition") : Action(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;

    static bool CanBuyPetition(Player* bot);
};

class PetitionOfferAction : public Action
{
public:
    PetitionOfferAction(PlayerbotAI* botAI, std::string const& name = "petition offer") : Action(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class PetitionOfferNearbyAction : public PetitionOfferAction
{
public:
    PetitionOfferNearbyAction(PlayerbotAI* botAI) : PetitionOfferAction(botAI, "offer petition nearby") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class PetitionTurnInAction : public Action
{
public:
    PetitionTurnInAction(PlayerbotAI* botAI, std::string const& name = "turn in petition") : Action(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class BuyTabardAction : public Action
{
public:
    BuyTabardAction(PlayerbotAI* botAI, std::string const& name = "buy tabard") : Action(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

#endif

