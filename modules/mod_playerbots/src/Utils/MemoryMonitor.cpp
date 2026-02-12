#include "MemoryMonitor.h"

#include <algorithm>
#include <cstdio>
#include <sstream>
#include <vector>

#include "Log.h"
#include "RandomPlayerbotMgr.h"
#include "Timer.h"

void MemoryMonitor::Add(std::string objectType, uint64 object, int level, std::string stack)
{
    if (objectType.empty())
        return;

    objectnumbers[std::this_thread::get_id()][objectType]++;

    if (level > 0 && static_cast<int>(object % static_cast<uint64>(level)) == 0)
    {
        if (stack.empty())
            stack = "tracked";
        adds[std::this_thread::get_id()][objectType][object] = std::make_pair(stack, time(nullptr));
    }
}

void MemoryMonitor::Rem(std::string objectType, uint64 object, int level)
{
    if (objectType.empty())
        return;

    objectnumbers[std::this_thread::get_id()][objectType]--;

    if (level > 0 && static_cast<int>(object % static_cast<uint64>(level)) == 0)
    {
        auto& trackedAdds = adds[std::this_thread::get_id()][objectType];
        auto itr = trackedAdds.find(object);
        if (itr != trackedAdds.end())
            trackedAdds.erase(itr);
        else
            rems[std::this_thread::get_id()][objectType][object] = std::make_pair("rem", time(nullptr));
    }
}

void MemoryMonitor::Print()
{
    Browse();

    std::map<std::string, int64> nums;
    for (auto const& [tid, byType] : objectnumbers)
    {
        (void)tid;
        for (auto const& [type, value] : byType)
            nums[type] += value;
    }

    objectnumbersHist.push_back(nums);
    if (objectnumbersHist.size() > 10)
        objectnumbersHist.pop_front();

    for (auto const& [type, value] : nums)
    {
        int64 delta = 0;
        if (objectnumbersHist.size() >= 2)
            delta = objectnumbersHist.back()[type] - objectnumbersHist.front()[type];

        TC_LOG_INFO("playerbots", "MEMORY %s : " UI64FMTD " (" UI64FMTD ")", type.c_str(), static_cast<uint64>(value), static_cast<uint64>(delta));
    }
}

void MemoryMonitor::LogCount(std::string filename)
{
    if (filename.empty())
        return;

    FILE* file = fopen(filename.c_str(), headers ? "a" : "w");
    if (!file)
        return;

    std::map<std::string, int64> nums;
    for (auto const& [tid, byType] : objectnumbers)
    {
        (void)tid;
        for (auto const& [type, value] : byType)
            nums[type] += value;
    }

    if (!headers)
    {
        std::ostringstream header;
        header << "time,onlinebots";
        for (auto const& [type, value] : nums)
        {
            (void)value;
            header << "," << type;
        }
        fprintf(file, "%s\n", header.str().c_str());
        headers = true;
    }

    std::ostringstream line;
    line << getMSTime() << "," << sRandomPlayerbotMgr->GetPlayerbotsCount();
    for (auto const& [type, value] : nums)
    {
        (void)type;
        line << "," << value;
    }

    fprintf(file, "%s\n", line.str().c_str());
    fclose(file);
}

void MemoryMonitor::Browse()
{
    std::unordered_map<std::string, std::unordered_map<std::string, uint32>> leaks;
    std::unordered_map<std::string, std::vector<uint64>> cleaned;

    for (auto& [tid, byType] : adds)
    {
        for (auto& [type, objects] : byType)
        {
            for (auto const& [objectId, createInfo] : objects)
            {
                bool stillHere = true;
                for (auto& [rtid, remByType] : rems)
                {
                    (void)rtid;
                    auto remTypeItr = remByType.find(type);
                    if (remTypeItr == remByType.end())
                        continue;

                    if (remTypeItr->second.find(objectId) != remTypeItr->second.end())
                    {
                        stillHere = false;
                        break;
                    }
                }

                if (stillHere)
                    leaks[type][createInfo.first]++;
                else
                    cleaned[type].push_back(objectId);
            }
        }
    }

    for (auto& [tid, byType] : adds)
    {
        for (auto& [type, objects] : byType)
        {
            auto const cleanItr = cleaned.find(type);
            if (cleanItr == cleaned.end())
                continue;

            for (uint64 objectId : cleanItr->second)
            {
                objects.erase(objectId);
                rems[tid][type].erase(objectId);
            }
        }
    }

    for (auto const& [type, stacks] : leaks)
    {
        TC_LOG_INFO("playerbots", "[%s] = %u", type.c_str(), static_cast<uint32>(stacks.size()));

        std::vector<std::pair<std::string, uint32>> ordered(stacks.begin(), stacks.end());
        std::sort(ordered.begin(), ordered.end(),
            [](std::pair<std::string, uint32> const& l, std::pair<std::string, uint32> const& r)
            {
                return l.second > r.second;
            });

        uint32 shown = 0;
        for (auto const& [stack, count] : ordered)
        {
            TC_LOG_INFO("playerbots", "<%u> %s", count, stack.c_str());
            if (++shown >= 10)
                break;
        }
    }
}

