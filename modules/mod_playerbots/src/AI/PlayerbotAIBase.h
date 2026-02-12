

#ifndef _PLAYERBOT_PLAYERBOTAIBASE_H
#define _PLAYERBOT_PLAYERBOTAIBASE_H

#include <memory>

#include "Define.h"

class PlayerbotAIBase
{
public:
    PlayerbotAIBase(bool isBotAI);
    virtual ~PlayerbotAIBase();

    bool CanUpdateAI();
    void SetNextCheckDelay(uint32 const delay);
    void IncreaseNextCheckDelay(uint32 delay);
    void YieldThread(uint32 delay = 100);
    virtual void UpdateAI(uint32 elapsed, bool minimal = false);
    virtual void UpdateAIInternal(uint32 elapsed, bool minimal = false) = 0;
    bool IsActive();
    bool IsBotAI() const;

protected:
    uint32 nextAICheckDelay;
    std::unique_ptr<class PerformanceMonitorOperation> totalPmo;

private:
    bool _isBotAI;
};

#endif
