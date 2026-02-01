

#ifndef _PLAYERBOT_PLAYERbotAIAWARE_H
#define _PLAYERBOT_PLAYERbotAIAWARE_H

class PlayerbotAI;

class PlayerbotAIAware
{
public:
    PlayerbotAIAware(PlayerbotAI* botAI) : botAI(botAI) {}

protected:
    PlayerbotAI* botAI;
};

#endif
