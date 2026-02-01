#ifndef _PLAYERBOT_CHATCOMMANDHANDLERSTRATEGY_H
#define _PLAYERBOT_CHATCOMMANDHANDLERSTRATEGY_H

#include "PassTroughStrategy.h"

class PlayerbotAI;

class ChatCommandHandlerStrategy : public PassTroughStrategy
{
public:
    ChatCommandHandlerStrategy(PlayerbotAI* botAI);

    std::string const getName() override { return "chat"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

#endif
