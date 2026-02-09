/*
* This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _PLAYERBOT_WORLD_THREAD_PROCESSOR_H
#define _PLAYERBOT_WORLD_THREAD_PROCESSOR_H

#include <memory>
#include <mutex>
#include <queue>

#include "Log.h"
#include "PlayerbotOperation.h"

/**
 * @brief Processes thread-unsafe bot operations in the world thread
 *
 * The PlayerbotWorldThreadProcessor manages a queue of operations that must be executed
 * in the world thread rather than map threads. This ensures thread safety for operations
 * like group modifications, LFG, guilds, battlegrounds, etc.
 */
class PlayerbotWorldThreadProcessor
{
public:
    static PlayerbotWorldThreadProcessor& instance()
    {
        static PlayerbotWorldThreadProcessor instance;
        return instance;
    }

    /**
     * @brief Update and process queued operations (called from world thread)
     *
     * This method should be called from WorldScript::OnUpdate hook, which runs in the world thread.
     *
     * @param diff Time since last update in milliseconds
     */
    void Update(uint32 diff);

    /**
     * @brief Queue an operation for execution in the world thread
     *
     * Thread-safe method that can be called from any thread (typically map threads).
     * The operation will be executed later during Update().
     *
     * @param operation Unique pointer to the operation (ownership is transferred)
     * @return true if operation was queued, false if queue is full
     */
    bool QueueOperation(std::unique_ptr<PlayerbotOperation> operation);

    /**
     * @brief Get current queue size
     */
    uint32 GetQueueSize() const;

    /**
     * @brief Clear all queued operations
     */
    void ClearQueue();

    /**
     * @brief Get statistics about operation processing
     */
    struct Statistics
    {
        uint64 totalOperationsProcessed = 0;
        uint64 totalOperationsFailed = 0;
        uint64 totalOperationsSkipped = 0;
        uint32 currentQueueSize = 0;
        uint32 maxQueueSize = 0;
        uint32 averageExecutionTimeMs = 0;
    };

    Statistics GetStatistics() const;

    /**
     * @brief Enable/disable operation processing
     */
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }

private:
    PlayerbotWorldThreadProcessor()
        : m_enabled(true),
          m_maxQueueSize(10000),
          m_batchSize(100),
          m_queueWarningThreshold(80),
          m_timeSinceLastUpdate(0),
          m_updateInterval(50)
    {
        TC_LOG_INFO("playerbots", "PlayerbotWorldThreadProcessor initialized");
    }
    ~PlayerbotWorldThreadProcessor() { ClearQueue(); }

    void ProcessBatch();
    void CheckQueueHealth();

    mutable std::mutex m_queueMutex;
    std::queue<std::unique_ptr<PlayerbotOperation>> m_operationQueue;

    bool m_enabled;
    uint32 m_maxQueueSize;
    uint32 m_batchSize;
    uint32 m_queueWarningThreshold;

    mutable std::mutex m_statsMutex;
    Statistics m_stats;

    uint32 m_timeSinceLastUpdate;
    uint32 m_updateInterval;
};

#endif
