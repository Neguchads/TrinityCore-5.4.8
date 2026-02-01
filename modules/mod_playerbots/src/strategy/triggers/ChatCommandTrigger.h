#ifndef _PLAYERBOT_CHATCOMMANDTRIGGER_H
#define _PLAYERBOT_CHATCOMMANDTRIGGER_H

#include "Trigger.h"

class Event;
class Player;
class PlayerbotAI;

class ChatCommandTrigger : public Trigger
{
public:
    ChatCommandTrigger(PlayerbotAI* botAI, std::string const command);

    void ExternalEvent(std::string const param, Player* owner = nullptr) override;
    Event Check() override;
    void Reset() override;

private:
    std::string param;
    bool triggered;
    Player* owner;
};

#endif
