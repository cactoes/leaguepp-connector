#include "include/connector.hpp"

#include <thread>
#include <unordered_map>
#include <vector>

#include "wsclient.hpp"
#include "httpclient.hpp"

#include "process.hpp"
#include "lockfile.hpp"
#include "connector.hpp"

static std::thread g_connectionThread;
static bool g_keepAlive;
static bool g_isReady = false;
static connector::connection_state g_currentState = connector::connection_state::NONE;

struct handlers_t {
    std::function<void(void)> m_onConnect = nullptr;
    std::function<void(void)> m_onDisconnect = nullptr;
    std::unordered_map<std::string, std::vector<std::function<void(nlohmann::json)>>> m_eventHandlers;
} g_handlers;

#define SLEEP(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

void ConnectionWatcher() {
    static riot::lock_file_t s_lockFile = {};
    static bool s_isConnected = false;

    wsclient::SetConnectHandler([]() {
        s_isConnected = true;

        if (g_handlers.m_onConnect)
            g_handlers.m_onConnect();
    });

    wsclient::SetDisconnectHandler([]() {
        s_isConnected = false;

        httpclient::Destroy();
        s_lockFile = {};

        if (g_handlers.m_onDisconnect)
            g_handlers.m_onDisconnect();
    });

    wsclient::SetMessageHandler([](const std::string& uri, const nlohmann::json& data) {
        if (g_handlers.m_eventHandlers.contains(uri))
            for (const auto listener : g_handlers.m_eventHandlers.at(uri))
                listener(data);
    });

    while (g_keepAlive) {
        // connected nothing to do
        if (s_isConnected) {
            SLEEP(1000);
            continue;
        }

        // disconnected and possible lockfile
        if (!riot::IsValidLockFile(s_lockFile)) {
            if (!riot::GetLockFile(s_lockFile)) {
                SLEEP(1000);
                continue;
            }
            httpclient::CreateClient(s_lockFile);
        }

        // connect
        nlohmann::json tmp;
        if (httpclient::MakeRequest(httpclient::request_type::GET, "/lol-gameflow/v1/gameflow-phase", tmp) == 200)
            wsclient::CreateAndStart(&s_lockFile);

        SLEEP(1000);
    }

    httpclient::Destroy();
    wsclient::Destroy();
}

void connector::Connect(const config_t& config) {
    wsclient::SetWSLoggingState(config.enableWebSocketLogging);
    g_handlers.m_onConnect = config.connectHandler;
    g_handlers.m_onDisconnect = config.disconnectHandler;

    g_keepAlive = true;
    g_connectionThread = std::thread(ConnectionWatcher);
}

void connector::Disconnect() {
    g_keepAlive = false;
    g_connectionThread.join();
}

void connector::AddEventHandler(const std::string& endpoint, std::function<void(nlohmann::json)> listener) {
    g_handlers.m_eventHandlers[endpoint].push_back(listener);
}