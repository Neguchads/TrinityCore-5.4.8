#ifndef _PLAYERBOT_MEMORYMONITOR_H
#define _PLAYERBOT_MEMORYMONITOR_H

// Enable explicitly when needed.
// #define MEMORY_MONITOR

#include "Define.h"

#include <list>
#include <map>
#include <string>
#include <thread>
#include <unordered_map>

class MemoryMonitor
{
public:
    MemoryMonitor() = default;
    virtual ~MemoryMonitor() = default;
    static MemoryMonitor& instance()
    {
        static MemoryMonitor instance;
        return instance;
    }

public:
    // Object type is the class/struct name.
    // Object is an object address or unique identifier.
    // If level > 0, every (object % level == 0) add/rem is tracked in details.
    void Add(std::string objectType, uint64 object, int level = 0, std::string stack = "");
    void Rem(std::string objectType, uint64 object, int level = 0);
    void Print();
    void Browse();
    void LogCount(std::string filename);

private:
    std::unordered_map<std::thread::id, std::map<std::string, int64>> objectnumbers;
    std::list<std::map<std::string, int64>> objectnumbersHist;
    std::unordered_map<std::thread::id, std::unordered_map<std::string, std::unordered_map<uint64, std::pair<std::string, time_t>>>> adds;
    std::unordered_map<std::thread::id, std::unordered_map<std::string, std::unordered_map<uint64, std::pair<std::string, time_t>>>> rems;
    bool headers = false;
};

#define sMemoryMonitor MemoryMonitor::instance()

#endif
