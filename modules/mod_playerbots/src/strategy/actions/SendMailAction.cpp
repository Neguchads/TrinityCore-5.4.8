#include "SendMailAction.h"

#include "Event.h"
#include "PlayerbotAI.h"

bool SendMailAction::Execute(Event event)
{
    Player* owner = event.getOwner();
    std::string const param = event.getParam();
    if (param.empty())
        return botAI->DoSpecificAction("mail", Event("mail", "send", owner), true);

    return botAI->DoSpecificAction("mail", Event("mail", "send " + param, owner), true);
}

