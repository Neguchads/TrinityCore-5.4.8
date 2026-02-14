#ifndef MOD_PLAYERBOTS_PLAYERBOT_LLM_INTERFACE_H
#define MOD_PLAYERBOTS_PLAYERBOT_LLM_INTERFACE_H

#include "Common.h"

#include <atomic>
#include <string>
#include <vector>

// Minimal HTTP(S) JSON POST helper for the CMaNGOS "ai chat" LLM feature.
// Implementation is intentionally lightweight (raw sockets + OpenSSL for TLS),
// and is executed from worker threads (never the world thread).
class PlayerbotLLMInterface
{
public:
    struct UrlParts
    {
        bool valid = false;
        bool https = false;
        std::string host;
        uint16 port = 0;
        std::string target;
    };

    static UrlParts ParseUrl(std::string const& endpoint);

    // Returns false on network/transport failure (outResponseBody is undefined then).
    static bool HttpPostJson(UrlParts const& url, std::string const& apiKey, std::string const& jsonBody,
        uint32 timeoutSeconds, std::string& outResponseBody);

    // Extract lines from a response body using regex patterns from config.
    static std::vector<std::string> ParseResponse(std::string const& responseBody,
        std::string const& startPattern, std::string const& endPattern,
        std::string const& deletePattern, std::string const& splitPattern);

    static void LimitContext(std::string& context, size_t maxLen);

    // Global concurrency limiter (best-effort).
    static bool TryAcquireGenerationSlot(uint32 maxConcurrent);
    static void ReleaseGenerationSlot();

private:
    static std::atomic<uint32> _activeGenerations;
};

#endif // MOD_PLAYERBOTS_PLAYERBOT_LLM_INTERFACE_H
