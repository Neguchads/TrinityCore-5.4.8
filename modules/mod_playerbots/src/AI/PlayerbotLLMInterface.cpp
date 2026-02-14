#include "PlayerbotLLMInterface.h"

#include "Log.h"

#include <chrono>
#include <cstring>
#include <regex>
#include <sstream>
#include <thread>

#include <openssl/err.h>
#include <openssl/ssl.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

std::atomic<uint32> PlayerbotLLMInterface::_activeGenerations{0};

namespace
{
void ReplaceAll(std::string& s, std::string const& from, std::string const& to)
{
    if (from.empty())
        return;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos)
    {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
}

std::string GetSSLError()
{
    unsigned long err = ERR_get_error();
    char buf[256];
    ERR_error_string_n(err, buf, sizeof(buf));
    return std::string(buf);
}

#ifdef _WIN32
void CloseSocket(SOCKET s) { closesocket(s); }
#else
void CloseSocket(int s) { close(s); }
#endif

#ifdef _WIN32
void SetNonBlockingSocket(SOCKET sock)
{
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);
}

void RestoreBlockingSocket(SOCKET sock)
{
    u_long mode = 0;
    ioctlsocket(sock, FIONBIO, &mode);
}
#else
void SetNonBlockingSocket(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
        return;
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

void RestoreBlockingSocket(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
        return;
    fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
}
#endif

template <typename SockT>
std::string RecvWithTimeout(SockT sock, uint32 timeoutSeconds, int& bytesRead)
{
    char buffer[4096];
    std::string response;

    SetNonBlockingSocket(sock);
    auto start = std::chrono::steady_clock::now();

    while (true)
    {
#ifdef _WIN32
        bytesRead = recv(sock, buffer, static_cast<int>(sizeof(buffer) - 1), 0);
#else
        bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
#endif
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            response += buffer;
            continue;
        }

        if (bytesRead == 0)
            break;

        // Would block?
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN)
#endif
        {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >= timeoutSeconds)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        break;
    }

    RestoreBlockingSocket(sock);
    return response;
}

std::string SSLRecvWithTimeout(SSL* ssl, uint32 timeoutSeconds, int& bytesRead)
{
    char buffer[4096];
    std::string response;

    auto start = std::chrono::steady_clock::now();
    while (true)
    {
        bytesRead = SSL_read(ssl, buffer, static_cast<int>(sizeof(buffer) - 1));
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            response += buffer;
            continue;
        }

        int sslErr = SSL_get_error(ssl, bytesRead);
        if (sslErr == SSL_ERROR_WANT_READ || sslErr == SSL_ERROR_WANT_WRITE)
        {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >= timeoutSeconds)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        break;
    }

    return response;
}

std::string ExtractAfterPattern(std::string const& content, std::string const& startPattern)
{
    if (startPattern.empty())
        return content;

    std::regex pattern(startPattern);
    std::smatch match;
    if (!std::regex_search(content, match, pattern))
        return content;

    size_t startPos = match.position() + match.length();
    if (startPos >= content.size())
        return "";
    return content.substr(startPos);
}

std::string ExtractBeforePattern(std::string const& content, std::string const& endPattern)
{
    if (endPattern.empty())
        return content;

    std::regex pattern(endPattern);
    std::smatch match;
    if (!std::regex_search(content, match, pattern))
        return content;

    size_t endPos = match.position();
    return content.substr(0, endPos);
}

std::vector<std::string> SplitResponse(std::string const& response, std::string const& splitPattern)
{
    std::vector<std::string> result;
    if (splitPattern.empty())
    {
        result.push_back(response);
        return result;
    }

    std::regex pattern(splitPattern);
    for (std::sregex_iterator it(response.begin(), response.end(), pattern), end; it != end; ++it)
        result.push_back(it->str());

    if (result.empty())
        result.push_back(response);

    return result;
}
}

PlayerbotLLMInterface::UrlParts PlayerbotLLMInterface::ParseUrl(std::string const& endpoint)
{
    UrlParts p;
    p.target = "/";

    try
    {
        std::regex urlRegex(R"((http|https)://([^:/]+)(:([0-9]+))?(/.*)?)");
        std::smatch match;
        if (!std::regex_match(endpoint, match, urlRegex))
            return p;

        p.https = (match[1] == "https");
        p.host = match[2].str();

        if (match[4].matched && !match[4].str().empty())
            p.port = static_cast<uint16>(std::stoi(match[4].str()));
        else
            p.port = p.https ? 443 : 80;

        if (match[5].matched && !match[5].str().empty())
            p.target = match[5].str();

        p.valid = !p.host.empty() && p.port != 0;
        return p;
    }
    catch (...)
    {
        return UrlParts{};
    }
}

bool PlayerbotLLMInterface::HttpPostJson(UrlParts const& url, std::string const& apiKey, std::string const& jsonBody,
    uint32 timeoutSeconds, std::string& outResponseBody)
{
    outResponseBody.clear();
    if (!url.valid)
        return false;

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        TC_LOG_ERROR("playerbots", "BotLLM: WSAStartup failed");
        return false;
    }
#endif

    addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* res = nullptr;
    std::string portStr = std::to_string(url.port);
    if (getaddrinfo(url.host.c_str(), portStr.c_str(), &hints, &res) != 0 || !res)
    {
        TC_LOG_ERROR("playerbots", "BotLLM: Failed to resolve hostname: %s", url.host.c_str());
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

#ifdef _WIN32
    SOCKET sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == INVALID_SOCKET)
    {
        TC_LOG_ERROR("playerbots", "BotLLM: Socket creation failed. Error: %d", WSAGetLastError());
        freeaddrinfo(res);
        WSACleanup();
        return false;
    }
#else
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0)
    {
        TC_LOG_ERROR("playerbots", "BotLLM: Socket creation failed. Error: %s", strerror(errno));
        freeaddrinfo(res);
        return false;
    }
#endif

    if (connect(sock, res->ai_addr, static_cast<int>(res->ai_addrlen)) < 0)
    {
#ifdef _WIN32
        TC_LOG_ERROR("playerbots", "BotLLM: Connection failed. Error: %d", WSAGetLastError());
        CloseSocket(sock);
        freeaddrinfo(res);
        WSACleanup();
#else
        TC_LOG_ERROR("playerbots", "BotLLM: Connection failed. Error: %s", strerror(errno));
        CloseSocket(sock);
        freeaddrinfo(res);
#endif
        return false;
    }

    freeaddrinfo(res);

    SSL_CTX* ctx = nullptr;
    SSL* ssl = nullptr;
    if (url.https)
    {
        static bool sslInitialized = false;
        if (!sslInitialized)
        {
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_all_algorithms();
            sslInitialized = true;
        }

        ctx = SSL_CTX_new(TLS_client_method());
        if (!ctx)
        {
            TC_LOG_ERROR("playerbots", "BotLLM: Failed to create SSL context: %s", GetSSLError().c_str());
            CloseSocket(sock);
#ifdef _WIN32
            WSACleanup();
#endif
            return false;
        }

        SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
        SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

        ssl = SSL_new(ctx);
        if (!ssl)
        {
            TC_LOG_ERROR("playerbots", "BotLLM: Failed to create SSL object: %s", GetSSLError().c_str());
            SSL_CTX_free(ctx);
            CloseSocket(sock);
#ifdef _WIN32
            WSACleanup();
#endif
            return false;
        }

        SSL_set_tlsext_host_name(ssl, url.host.c_str());
        SSL_set_fd(ssl, static_cast<int>(sock));

        int r = SSL_connect(ssl);
        if (r <= 0)
        {
            TC_LOG_ERROR("playerbots", "BotLLM: SSL_connect failed: %s", GetSSLError().c_str());
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            CloseSocket(sock);
#ifdef _WIN32
            WSACleanup();
#endif
            return false;
        }
    }

    std::ostringstream req;
    req << "POST " << (url.target.empty() ? "/" : url.target) << " HTTP/1.1\r\n";
    req << "Host: " << url.host << "\r\n";
    req << "Content-Type: application/json\r\n";
    req << "Accept: application/json\r\n";
    req << "Connection: close\r\n";
    if (!apiKey.empty())
        req << "Authorization: Bearer " << apiKey << "\r\n";
    req << "Content-Length: " << jsonBody.size() << "\r\n";
    req << "\r\n";
    req << jsonBody;

    std::string reqStr = req.str();

    int writeResult = 0;
    if (url.https && ssl)
        writeResult = SSL_write(ssl, reqStr.c_str(), static_cast<int>(reqStr.size()));
    else
        writeResult = send(sock, reqStr.c_str(), static_cast<int>(reqStr.size()), 0);

    if ((url.https && writeResult <= 0) || (!url.https && writeResult < 0))
    {
        if (url.https && ssl)
        {
            SSL_shutdown(ssl);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
        }
        CloseSocket(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

    int bytesRead = 0;
    std::string rawResponse;
    if (url.https && ssl)
        rawResponse = SSLRecvWithTimeout(ssl, timeoutSeconds, bytesRead);
    else
        rawResponse = RecvWithTimeout(sock, timeoutSeconds, bytesRead);

    if (url.https && ssl)
    {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
    }

    CloseSocket(sock);
#ifdef _WIN32
    WSACleanup();
#endif

    // Strip HTTP headers if present.
    size_t pos = rawResponse.find("\r\n\r\n");
    if (pos != std::string::npos)
        outResponseBody = rawResponse.substr(pos + 4);
    else
        outResponseBody = rawResponse;

    return true;
}

std::vector<std::string> PlayerbotLLMInterface::ParseResponse(std::string const& responseBody,
    std::string const& startPattern, std::string const& endPattern,
    std::string const& deletePattern, std::string const& splitPattern)
{
    std::string s = responseBody;

    try
    {
        s = ExtractAfterPattern(s, startPattern);

        // Reduce escaping noise for in-game output.
        ReplaceAll(s, R"(\")", "'");

        s = ExtractBeforePattern(s, endPattern);

        if (!deletePattern.empty())
            s = std::regex_replace(s, std::regex(deletePattern), "");

        return SplitResponse(s, splitPattern);
    }
    catch (std::exception const& e)
    {
        TC_LOG_ERROR("playerbots", "BotLLM: ParseResponse failed: %s", e.what());
        return {};
    }
}

void PlayerbotLLMInterface::LimitContext(std::string& context, size_t maxLen)
{
    if (!maxLen)
        return;

    if (context.size() <= maxLen)
        return;

    size_t cutNeeded = context.size() - maxLen;
    if (cutNeeded >= context.size())
    {
        context.clear();
        return;
    }

    size_t cutPos = cutNeeded;
    for (size_t i = cutNeeded; i < context.size(); ++i)
    {
        if (context[i] == ' ' || context[i] == '.')
        {
            cutPos = i + 1;
            break;
        }
    }

    if (cutPos < context.size())
        context = context.substr(cutPos);
    else
        context.clear();
}

bool PlayerbotLLMInterface::TryAcquireGenerationSlot(uint32 maxConcurrent)
{
    if (!maxConcurrent)
        return true;

    uint32 cur = _activeGenerations.load(std::memory_order_relaxed);
    while (true)
    {
        if (cur >= maxConcurrent)
            return false;
        if (_activeGenerations.compare_exchange_weak(cur, cur + 1, std::memory_order_acq_rel, std::memory_order_relaxed))
            return true;
    }
}

void PlayerbotLLMInterface::ReleaseGenerationSlot()
{
    uint32 cur = _activeGenerations.load(std::memory_order_relaxed);
    while (true)
    {
        if (cur == 0)
            return;
        if (_activeGenerations.compare_exchange_weak(cur, cur - 1, std::memory_order_acq_rel, std::memory_order_relaxed))
            return;
    }
}

