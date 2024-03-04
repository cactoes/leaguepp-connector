#include "include/connector.hpp"

#include <thread>
#include <unordered_map>
#include <vector>

#include "wsclient.hpp"
#include "httpclient.hpp"

#include "process.hpp"
#include "lockfile.hpp"
#include "connector.hpp"

std::thread g_connectionThread;
bool g_keepAlive;
bool g_isReady = false;
int g_reconnectInterval;

struct handlers_t {
    std::function<void(void)> connect = nullptr;
    std::function<void(void)> disconnect = nullptr;
    std::unordered_map<std::string, std::vector<std::function<void(nlohmann::json)>>> eventHandlers;
} g_handlers;

#define SLEEP(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

void ConnectionWatcher() {
    static riot::lock_file_t s_lockFile = {};
    static bool s_isConnected = false;

    wsclient::SetConnectHandler([]() {
        s_isConnected = true;

        if (g_handlers.connect)
            g_handlers.connect();
    });

    wsclient::SetDisconnectHandler([]() {
        s_isConnected = false;

        httpclient::Destroy();
        s_lockFile = {};

        if (g_handlers.disconnect)
            g_handlers.disconnect();
    });

    wsclient::SetMessageHandler([](const std::string& uri, const nlohmann::json& data) {
        if (g_handlers.eventHandlers.contains(uri))
            for (const auto listener : g_handlers.eventHandlers.at(uri))
                listener(data);
    });

    while (g_keepAlive) {
        if (s_isConnected) {
            SLEEP(g_reconnectInterval);
            continue;
        }

        if (!riot::IsValidLockFile(s_lockFile)) {
            if (!riot::GetLockFile(s_lockFile)) {
                SLEEP(g_reconnectInterval);
                continue;
            }
            httpclient::CreateClient(s_lockFile);
        }

        nlohmann::json tmp;
        if (httpclient::GetRequest("/lol-gameflow/v1/gameflow-phase", tmp) == 200)
            wsclient::CreateAndStart(&s_lockFile);

        SLEEP(g_reconnectInterval);
    }

    httpclient::Destroy();
    wsclient::Destroy();
}

void connector::Connect(const config_t& config) {
    wsclient::SetWSLoggingState(config.enableWebSocketLogging);
    g_handlers.connect = config.connectHandler;
    g_handlers.disconnect = config.disconnectHandler;
    g_reconnectInterval = config.reconnectInterval;

    g_keepAlive = true;
    g_connectionThread = std::thread(ConnectionWatcher);
}

void connector::Disconnect() {
    g_keepAlive = false;
    g_connectionThread.join();
}

void connector::AddEventHandler(const std::string& endpoint, std::function<void(nlohmann::json)> listener) {
    g_handlers.eventHandlers[endpoint].push_back(listener);
}

connector::result_t connector::MakeRequest(request_type type, const std::string& endpoint, const std::string& data) {
    connector::result_t result = {};

    switch (type) {
        case request_type::GET:
            result.status = httpclient::GetRequest(endpoint, result.data);
            break;
        case request_type::PUT:
            result.status = httpclient::PutRequest(endpoint, result.data, data);
            break;
        case request_type::POST:
            result.status = httpclient::PostRequest(endpoint, result.data, data);
            break;
        case request_type::PATCH:
            result.status = httpclient::PatchRequest(endpoint, result.data, data);
            break;
        case request_type::DEL:
            result.status = httpclient::DeleteRequest(endpoint, result.data, data);
            break;
        default:
            result.status = -1;
            break;
    }

    return result;
}