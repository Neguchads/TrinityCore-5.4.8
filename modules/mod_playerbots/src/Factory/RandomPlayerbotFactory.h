

#ifndef _PLAYERBOT_RANDOMPLAYERBOTFACTORY_H
#define _PLAYERBOT_RANDOMPLAYERBOTFACTORY_H

#include <map>
#include <unordered_map>
#include <vector>

#include "Common.h"
#include "DBCEnums.h"

class Player;
class WorldSession;

class RandomPlayerbotFactory
{
public:
    RandomPlayerbotFactory(uint32 accountId);
    virtual ~RandomPlayerbotFactory() {}

    Player* CreateRandomBot(WorldSession* session, Classes cls, std::unordered_map<Gender, std::vector<std::string>>& names);
    static void CreateRandomBots();

private:
    std::string const CreateRandomBotName(Gender g);


    uint32 accountId;
    static std::map<uint8, std::vector<uint8>> availableRaces;
};

#endif
