#include "wsclient.hpp"

#include <mutex>
#include <thread>
#include <memory>

#define DBG_LOG(...) if (g_enableWSLogging) std::cout << __VA_ARGS__ << "\n"

wsclient::client_handlers_t g_handlers = {};
bool g_enableWSLogging = true;
std::unique_ptr<wsclient::client> g_client;

bool wsclient::HasValidHandlers(wsclient::client_handlers_t* handlers) {
    return handlers->connect && handlers->disconnect && handlers->message;
}

void wsclient::client::SetLockFile(riot::lock_file_t* lockFile) {
    m_lockFile = lockFile;
}

void wsclient::client::Connect() {
    m_thread = std::thread(&client::InternalRunner, this);
    m_hasThread = true;
}

void wsclient::client::Disconnect() {
    if (m_hasThread) {
        m_hasThread = false;

        websocketpp::lib::error_code ec;
        m_client.close(m_connection->get_handle(), websocketpp::close::status::going_away, "", ec);
        if (ec) { DBG_LOG("m_client.close: " << ec.message()); }

        m_thread.join();
    }
}

void wsclient::client::InternalRunner() {
    if (g_enableWSLogging) {
        m_client.set_access_channels(websocketpp::log::alevel::all);
        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
        m_client.set_error_channels(websocketpp::log::elevel::all);
    } else {
        m_client.set_access_channels(websocketpp::log::alevel::none);
        m_client.clear_access_channels(websocketpp::log::alevel::none);
        m_client.set_error_channels(websocketpp::log::elevel::none);
    }

    m_client.init_asio();

    m_client.set_open_handler([this](websocketpp::connection_hdl hdl) {
        m_client.get_con_from_hdl(hdl)->send("[5,\"OnJsonApiEvent\"]");
        g_handlers.connect();
    });

    m_client.set_close_handler([this](websocketpp::connection_hdl) {
        g_handlers.disconnect();
    });

    m_client.set_tls_init_handler([](websocketpp::connection_hdl) {
        using boost::asio::ssl::context;
        return websocketpp::lib::make_shared<context>(context::tlsv12_client);
    });

    m_client.set_message_handler([](websocketpp::connection_hdl, wsclient::wsclient_t::message_ptr msg) {
        auto obj = nlohmann::json::parse(msg->get_payload(), nullptr, false);

        if (!obj.is_discarded()) {
            auto result = obj.at(2).get<api_result_t>();
            g_handlers.message(result.uri, result.data);
        }
    });

    m_client.set_open_handshake_timeout(900);
    m_client.set_close_handshake_timeout(900);
    m_client.set_pong_timeout(900);

    websocketpp::lib::error_code ec;
    m_connection = m_client.get_connection("ws://127.0.0.1:" + m_lockFile->m_port, ec);
    if (ec) {
        DBG_LOG("could not create connection because: " << ec.message());
        return;
    }

    m_connection->append_header("Authorization", "Basic " + riot::EncodeLockFile(*m_lockFile));
    m_connection->set_open_handshake_timeout(900);
    m_connection->set_close_handshake_timeout(900);
    m_connection->set_pong_timeout(900);

    try {
        m_client.connect(m_connection);
        m_client.run();
    } catch(const websocketpp::exception& e) {
        DBG_LOG("failed to start client: " << e.what());
    }
}

void wsclient::SetWSLoggingState(bool state) {
    g_enableWSLogging = state;
}

void wsclient::SetConnectHandler(wsclient::connect_handler_t handler) {
    g_handlers.connect = handler;
}

void wsclient::SetDisconnectHandler(wsclient::disconnect_handler_t handler) {
    g_handlers.disconnect = handler;
}

void wsclient::SetMessageHandler(wsclient::message_handler_t handler) {
    g_handlers.message = handler;
}

bool wsclient::CreateAndStart(riot::lock_file_t* lockFile) {
    if (!HasValidHandlers(&g_handlers))
        return false;

    if (g_client)
        g_client->Disconnect();

    g_client = std::make_unique<client>();
    g_client->SetLockFile(lockFile);
    g_client->Connect();

    return true;
}

void wsclient::Destroy() {
    if (g_client)
        g_client->Disconnect();
}