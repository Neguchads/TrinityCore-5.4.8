#ifndef _PLAYERBOT_PETITIONSIGNACTION_H
#define _PLAYERBOT_PETITIONSIGNACTION_H

#include "Action.h"

// WorldPacket-driven action (SMSG_PETITION_SHOW_SIGNATURES).
class PetitionSignAction : public Action
{
public:
    PetitionSignAction(PlayerbotAI* botAI, std::string const& name = "petition sign") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

#endif

