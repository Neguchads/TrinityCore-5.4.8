

#ifndef _PLAYERBOT_CUSTOMSTRATEGY_H
#define _PLAYERBOT_CUSTOMSTRATEGY_H

#include <map>

#include "Strategy.h"

class PlayerbotAI;

class CustomStrategy : public Strategy, public Qualified
{
public:
    CustomStrategy(PlayerbotAI* botAI);

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    std::string const getName() override { return std::string("custom::" + qualifier); }
    void Reset();

    static std::map<std::string, std::string> actionLinesCache;

private:
    std::vector<std::string> actionLines;
    void LoadActionLines(uint32 owner);
};

#endif
