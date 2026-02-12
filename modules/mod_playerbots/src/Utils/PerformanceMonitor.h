#ifndef _PLAYERBOT_PERFORMANCEMONITOR_H
#define _PLAYERBOT_PERFORMANCEMONITOR_H

#include <chrono>
#include <ctime>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Common.h"

typedef std::vector<std::string> PerformanceStack;
class PlayerbotAI;

struct PerformanceData
{
    uint64 minTime;
    uint64 maxTime;
    uint64 totalTime;
    uint32 count;
    std::mutex lock;
};

enum PerformanceMetric
{
    PERF_MON_TRIGGER,
    PERF_MON_VALUE,
    PERF_MON_ACTION,
    PERF_MON_RNDBOT,
    PERF_MON_TOTAL
};

class PerformanceMonitorOperation
{
public:
    PerformanceMonitorOperation(std::vector<PerformanceData*> dataRefs, std::string const name, PerformanceStack* stack);
    ~PerformanceMonitorOperation();
    void finish();

private:
    std::vector<PerformanceData*> dataRefs;
    std::string const name;
    PerformanceStack* stack;
    std::chrono::microseconds started;
    bool finished = false;
};

using PerformanceNameMap = std::map<std::string, PerformanceData*>;
using PerformanceMetricMap = std::map<PerformanceMetric, PerformanceNameMap>;
using PerformanceInstanceMap = std::map<uint32, PerformanceMetricMap>;
using PerformanceMapMap = std::map<uint32, PerformanceInstanceMap>;

class PerformanceMonitor
{
public:
    PerformanceMonitor();
    virtual ~PerformanceMonitor();
    static PerformanceMonitor* instance()
    {
        static PerformanceMonitor instance;
        return &instance;
    }

public:
    std::unique_ptr<PerformanceMonitorOperation> start(
        PerformanceMetric metric,
        std::string const name,
        PerformanceStack* stack = nullptr,
        uint32 mapId = 0,
        uint32 instanceId = 0);
    std::unique_ptr<PerformanceMonitorOperation> start(PerformanceMetric metric, std::string const name, PlayerbotAI* ai);
    void Init(uint32 mapId = 0, uint32 instanceId = 0);
    void PrintStats(bool perTick = false, bool fullStack = false, bool showMap = false);
    void Reset();

private:
    static PerformanceData* GetOrCreateData(PerformanceNameMap& nameMap, std::string const& name);
    PerformanceMetricMap data;
    PerformanceMapMap mapsData;
    std::mutex lock;
};

#define sPerformanceMonitor PerformanceMonitor::instance()

#endif
