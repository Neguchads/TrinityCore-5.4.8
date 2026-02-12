#pragma once

#include "MovementActions.h"

class Player;
class PlayerbotAI;

class SummonAction : public MovementAction
{
public:
    SummonAction(PlayerbotAI* botAI, std::string const name = "summon") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;

private:
    bool Teleport(Player* summoner, Player* player);
};
