#include "PerformanceMonitor.h"

#include <algorithm>
#include <limits>
#include <sstream>

#include "Playerbots.h"

namespace
{
    std::string BuildStackName(std::string const& name, PerformanceStack* stack)
    {
        std::string stackName = name;
        if (!stack || stack->empty())
            return stackName;

        std::ostringstream out;
        out << stackName << " [";
        for (PerformanceStack::reverse_iterator itr = stack->rbegin(); itr != stack->rend(); ++itr)
            out << *itr << (std::next(itr) == stack->rend() ? "" : "|");
        out << "]";

        return out.str();
    }

    void ResetMetricMapData(PerformanceMetricMap& metricMap)
    {
        for (auto& metricItr : metricMap)
        {
            for (auto& nameItr : metricItr.second)
            {
                PerformanceData* pd = nameItr.second;
                if (!pd)
                    continue;

                std::lock_guard<std::mutex> guard(pd->lock);
                pd->minTime = 0;
                pd->maxTime = 0;
                pd->totalTime = 0;
                pd->count = 0;
            }
        }
    }

    struct SnapshotData
    {
        PerformanceMetricMap data;
        ~SnapshotData()
        {
            for (auto& metricItr : data)
            {
                for (auto& nameItr : metricItr.second)
                    delete nameItr.second;
            }
        }
    };

    std::string AppendMapSuffix(std::string const& name, uint32 mapId, uint32 instanceId)
    {
        std::ostringstream out;
        out << name << " " << mapId;
        if (instanceId)
            out << " (" << instanceId << ")";
        return out.str();
    }
}

PerformanceMonitor::PerformanceMonitor() {}

PerformanceMonitor::~PerformanceMonitor() {}

PerformanceData* PerformanceMonitor::GetOrCreateData(PerformanceNameMap& nameMap, std::string const& name)
{
    PerformanceData*& pd = nameMap[name];
    if (!pd)
    {
        pd = new PerformanceData();
        pd->minTime = 0;
        pd->maxTime = 0;
        pd->totalTime = 0;
        pd->count = 0;
    }

    return pd;
}

std::unique_ptr<PerformanceMonitorOperation> PerformanceMonitor::start(
    PerformanceMetric metric,
    std::string const name,
    PerformanceStack* stack,
    uint32 mapId,
    uint32 instanceId)
{
    if (!sPlayerbotAIConfig->perfMonEnabled)
        return nullptr;

    std::string const stackName = BuildStackName(name, stack);
    if (stack)
        stack->push_back(name);

    std::lock_guard<std::mutex> guard(lock);

    std::vector<PerformanceData*> refs;
    refs.reserve(2);
    refs.push_back(GetOrCreateData(data[metric], stackName));

    PerformanceMapMap::iterator mapItr = mapsData.find(mapId);
    if (mapItr != mapsData.end())
    {
        PerformanceInstanceMap::iterator instanceItr = mapItr->second.find(instanceId);
        if (instanceItr != mapItr->second.end())
            refs.push_back(GetOrCreateData(instanceItr->second[metric], stackName));
    }

    return std::make_unique<PerformanceMonitorOperation>(std::move(refs), name, stack);
}

std::unique_ptr<PerformanceMonitorOperation> PerformanceMonitor::start(PerformanceMetric metric, std::string const name, PlayerbotAI* ai)
{
    if (!ai || !ai->GetAiObjectContext() || !ai->GetBot())
        return start(metric, name);

    return start(metric, name, &ai->GetAiObjectContext()->performanceStack, ai->GetBot()->GetMapId(),
                 ai->GetBot()->GetInstanceId());
}

void PerformanceMonitor::Init(uint32 mapId, uint32 instanceId)
{
    if (!sPlayerbotAIConfig->perfMonEnabled)
        return;

    std::lock_guard<std::mutex> guard(lock);
    mapsData[mapId][instanceId];
}

void PerformanceMonitor::PrintStats(bool perTick, bool fullStack, bool showMap)
{
    SnapshotData snapshotData;
    PerformanceMetricMap& snapshot = snapshotData.data;

    {
        std::lock_guard<std::mutex> guard(lock);

        auto AccumulateMetricMap = [&snapshot](PerformanceMetricMap const& src, bool appendMap, uint32 mapId, uint32 instanceId)
        {
            for (auto const& metricItr : src)
            {
                for (auto const& nameItr : metricItr.second)
                {
                    PerformanceData* srcPd = nameItr.second;
                    if (!srcPd)
                        continue;

                    uint64 minTime = 0;
                    uint64 maxTime = 0;
                    uint64 totalTime = 0;
                    uint32 count = 0;
                    {
                        std::lock_guard<std::mutex> dataGuard(srcPd->lock);
                        minTime = srcPd->minTime;
                        maxTime = srcPd->maxTime;
                        totalTime = srcPd->totalTime;
                        count = srcPd->count;
                    }

                    std::string displayName = nameItr.first;
                    if (appendMap)
                        displayName = AppendMapSuffix(displayName, mapId, instanceId);

                    PerformanceData* dstPd = PerformanceMonitor::GetOrCreateData(snapshot[metricItr.first], displayName);
                    if (totalTime > 0)
                    {
                        if (!dstPd->minTime || dstPd->minTime > minTime)
                            dstPd->minTime = minTime;
                        if (!dstPd->maxTime || dstPd->maxTime < maxTime)
                            dstPd->maxTime = maxTime;
                        dstPd->totalTime += totalTime;
                    }
                    dstPd->count += count;
                }
            }
        };

        if (showMap && !mapsData.empty())
        {
            for (auto const& mapItr : mapsData)
            {
                for (auto const& instanceItr : mapItr.second)
                    AccumulateMetricMap(instanceItr.second, true, mapItr.first, instanceItr.first);
            }
        }
        else
        {
            AccumulateMetricMap(data, false, 0, 0);
        }
    }

    if (snapshot.empty())
        return;

    if (!perTick)
    {
        float updateAITotalTime = 0.0f;
        for (auto const& totalItr : snapshot[PERF_MON_TOTAL])
            if (totalItr.first.find("PlayerbotAI::UpdateAIInternal") != std::string::npos)
                updateAITotalTime += static_cast<float>(totalItr.second->totalTime);

        if (updateAITotalTime <= 0.0f)
            return;

        TC_LOG_INFO(
            "playerbots",
            "--------------------------------------[TOTAL BOT]------------------------------------------------------");
        TC_LOG_INFO("playerbots",
                    "percentage     time  |     min ..     max (      avg  of      count) - type      : name");
        TC_LOG_INFO(
            "playerbots",
            "-------------------------------------------------------------------------------------------------------");

        for (auto itr = snapshot.begin(); itr != snapshot.end(); ++itr)
        {
            PerformanceNameMap const& pdMap = itr->second;

            std::string key;
            switch (itr->first)
            {
                case PERF_MON_TRIGGER:
                    key = "Trigger";
                    break;
                case PERF_MON_VALUE:
                    key = "Value";
                    break;
                case PERF_MON_ACTION:
                    key = "Action";
                    break;
                case PERF_MON_RNDBOT:
                    key = "RndBot";
                    break;
                case PERF_MON_TOTAL:
                    key = "Total";
                    break;
                default:
                    key = "?";
                    break;
            }

            std::vector<std::string> names;
            for (auto const& pdItr : pdMap)
            {
                if (key == "Total" && pdItr.first.find("PlayerbotAI::UpdateAIInternal") == std::string::npos)
                    continue;

                names.push_back(pdItr.first);
            }

            std::sort(names.begin(), names.end(),
                [&pdMap](std::string const& l, std::string const& r)
                {
                    return pdMap.at(l)->totalTime < pdMap.at(r)->totalTime;
                });

            uint64 typeTotalTime = 0;
            uint64 typeMinTime = std::numeric_limits<uint64>::max();
            uint64 typeMaxTime = 0;
            uint32 typeCount = 0;

            for (std::string const& name : names)
            {
                PerformanceData* pd = pdMap.at(name);
                if (!pd || !pd->count)
                    continue;

                typeTotalTime += pd->totalTime;
                typeCount += pd->count;
                typeMinTime = std::min(typeMinTime, pd->minTime);
                typeMaxTime = std::max(typeMaxTime, pd->maxTime);

                float perc = static_cast<float>(pd->totalTime) / updateAITotalTime * 100.0f;
                float time = static_cast<float>(pd->totalTime) / 1000000.0f;
                float minTime = static_cast<float>(pd->minTime) / 1000.0f;
                float maxTime = static_cast<float>(pd->maxTime) / 1000.0f;
                float avg = static_cast<float>(pd->totalTime) / static_cast<float>(pd->count) / 1000.0f;

                std::string disName = name;
                if (!fullStack && disName.find("|") != std::string::npos)
                    disName = disName.substr(0, disName.find("|")) + "]";

                if (perc >= 0.1f || avg >= 0.25f || pd->maxTime > 1000)
                {
                    TC_LOG_INFO("playerbots",
                                "%7.3f%% %10.3fs | %7.1f .. %7.1f (%10.3f of %10d) - %-6s : %s", perc, time,
                                minTime, maxTime, avg, pd->count, key.c_str(), disName.c_str());
                }
            }

            if (!typeCount)
            {
                TC_LOG_INFO("playerbots", " ");
                continue;
            }

            float tPerc = static_cast<float>(typeTotalTime) / updateAITotalTime * 100.0f;
            float tTime = static_cast<float>(typeTotalTime) / 1000000.0f;
            float tMinTime = static_cast<float>(typeMinTime) / 1000.0f;
            float tMaxTime = static_cast<float>(typeMaxTime) / 1000.0f;
            float tAvg = static_cast<float>(typeTotalTime) / static_cast<float>(typeCount) / 1000.0f;

            TC_LOG_INFO("playerbots", "%7.3f%% %10.3fs | %7.1f .. %7.1f (%10.3f of %10d) - %-6s : %s", tPerc,
                        tTime, tMinTime, tMaxTime, tAvg, typeCount, key.c_str(), "Total");
            TC_LOG_INFO("playerbots", " ");
        }
    }
    else
    {
        PerformanceNameMap& totalMap = snapshot[PERF_MON_TOTAL];
        auto fullTickItr = totalMap.find("PlayerbotAIBase::FullTick");
        if (fullTickItr == totalMap.end() || !fullTickItr->second || !fullTickItr->second->count || !fullTickItr->second->totalTime)
            return;

        float fullTickCount = static_cast<float>(fullTickItr->second->count);
        float fullTickTotalTime = static_cast<float>(fullTickItr->second->totalTime);

        TC_LOG_INFO(
            "playerbots",
            "---------------------------------------[PER TICK]------------------------------------------------------");
        TC_LOG_INFO("playerbots",
                    "percentage     time  |     min ..     max (      avg  of      count) - type      : name");
        TC_LOG_INFO(
            "playerbots",
            "-------------------------------------------------------------------------------------------------------");

        for (auto itr = snapshot.begin(); itr != snapshot.end(); ++itr)
        {
            PerformanceNameMap const& pdMap = itr->second;

            std::string key;
            switch (itr->first)
            {
                case PERF_MON_TRIGGER:
                    key = "Trigger";
                    break;
                case PERF_MON_VALUE:
                    key = "Value";
                    break;
                case PERF_MON_ACTION:
                    key = "Action";
                    break;
                case PERF_MON_RNDBOT:
                    key = "RndBot";
                    break;
                case PERF_MON_TOTAL:
                    key = "Total";
                    break;
                default:
                    key = "?";
                    break;
            }

            std::vector<std::string> names;
            for (auto const& pdItr : pdMap)
                names.push_back(pdItr.first);

            std::sort(names.begin(), names.end(),
                [&pdMap](std::string const& l, std::string const& r)
                {
                    return pdMap.at(l)->totalTime < pdMap.at(r)->totalTime;
                });

            uint64 typeTotalTime = 0;
            uint64 typeMinTime = std::numeric_limits<uint64>::max();
            uint64 typeMaxTime = 0;
            uint32 typeCount = 0;

            for (std::string const& name : names)
            {
                PerformanceData* pd = pdMap.at(name);
                if (!pd || !pd->count)
                    continue;

                typeTotalTime += pd->totalTime;
                typeCount += pd->count;
                typeMinTime = std::min(typeMinTime, pd->minTime);
                typeMaxTime = std::max(typeMaxTime, pd->maxTime);

                float perc = static_cast<float>(pd->totalTime) / fullTickTotalTime * 100.0f;
                float time = static_cast<float>(pd->totalTime) / fullTickCount / 1000.0f;
                float minTime = static_cast<float>(pd->minTime) / 1000.0f;
                float maxTime = static_cast<float>(pd->maxTime) / 1000.0f;
                float avg = static_cast<float>(pd->totalTime) / static_cast<float>(pd->count) / 1000.0f;
                float amount = static_cast<float>(pd->count) / fullTickCount;

                std::string disName = name;
                if (!fullStack && disName.find("|") != std::string::npos)
                    disName = disName.substr(0, disName.find("|")) + "]";

                if (perc >= 0.1f || avg >= 0.25f || pd->maxTime > 1000)
                {
                    TC_LOG_INFO("playerbots",
                                "%7.3f%% %9.3fms | %7.1f .. %7.1f (%10.3f of %10.2f) - %-6s : %s",
                                perc, time, minTime, maxTime, avg, amount, key.c_str(), disName.c_str());
                }
            }

            if (itr->first != PERF_MON_TOTAL && typeCount)
            {
                float tPerc = static_cast<float>(typeTotalTime) / fullTickTotalTime * 100.0f;
                float tTime = static_cast<float>(typeTotalTime) / fullTickCount / 1000.0f;
                float tMinTime = static_cast<float>(typeMinTime) / 1000.0f;
                float tMaxTime = static_cast<float>(typeMaxTime) / 1000.0f;
                float tAvg = static_cast<float>(typeTotalTime) / static_cast<float>(typeCount) / 1000.0f;
                float tAmount = static_cast<float>(typeCount) / fullTickCount;

                TC_LOG_INFO("playerbots", "%7.3f%% %9.3fms | %7.1f .. %7.1f (%10.3f of %10.2f) - %-6s : %s",
                            tPerc, tTime, tMinTime, tMaxTime, tAvg, tAmount, key.c_str(), "Total");
            }

            TC_LOG_INFO("playerbots", " ");
        }
    }
}

void PerformanceMonitor::Reset()
{
    std::lock_guard<std::mutex> guard(lock);
    ResetMetricMapData(data);
    for (auto& mapItr : mapsData)
        for (auto& instanceItr : mapItr.second)
            ResetMetricMapData(instanceItr.second);
}

PerformanceMonitorOperation::PerformanceMonitorOperation(
    std::vector<PerformanceData*> dataRefs,
    std::string const name,
    PerformanceStack* stack)
    : dataRefs(std::move(dataRefs)), name(name), stack(stack)
{
    started = (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()))
                  .time_since_epoch();
}

PerformanceMonitorOperation::~PerformanceMonitorOperation()
{
    finish();
}

void PerformanceMonitorOperation::finish()
{
    if (finished)
        return;

    finished = true;

    std::chrono::microseconds finishedAt =
        (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()))
            .time_since_epoch();
    uint64 elapsed = (finishedAt - started).count();

    for (PerformanceData* data : dataRefs)
    {
        if (!data)
            continue;

        std::lock_guard<std::mutex> guard(data->lock);
        if (elapsed > 0)
        {
            if (!data->minTime || data->minTime > elapsed)
                data->minTime = elapsed;

            if (!data->maxTime || data->maxTime < elapsed)
                data->maxTime = elapsed;

            data->totalTime += elapsed;
        }

        ++data->count;
    }

    if (stack)
        stack->erase(std::remove(stack->begin(), stack->end(), name), stack->end());
}
