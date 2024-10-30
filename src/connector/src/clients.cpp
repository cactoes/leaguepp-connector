#pragma warning(disable:4005)
#define CPPHTTPLIB_OPENSSL_SUPPORT
#pragma warning(default:4005)

#include <httplib.h>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>

#include "clients.hpp"

std::unique_ptr<httplib::Client> connector_http_client = nullptr;

void connector::http::create(const lock_file_t& lock_file) {
    connector_http_client = std::make_unique<httplib::Client>(lock_file_to_url(lock_file));
    connector_http_client->enable_server_certificate_verification(false);
    connector_http_client->set_basic_auth(lock_file.username, lock_file.password);
}

void connector::http::destroy() {
    connector_http_client = nullptr;
}

static int handle_request(httplib::Result result, std::string& out) {
    if (!result)
        return 0;

    // false, disable exceptions
    out = nlohmann::json::parse(result->body, nullptr, false).dump();

    return result->status;
}

int connector::http::get_request(const std::string& endpoint, std::string& out) {
    if (!connector_http_client)
        return -1;

    return handle_request(connector_http_client->Get(endpoint), out);
}

int connector::http::put_request(const std::string& endpoint, std::string& out, const std::string& in) {
    if (!connector_http_client)
        return -1;

    return handle_request(connector_http_client->Put(endpoint, in, "application/json"), out);
}

int connector::http::post_request(const std::string& endpoint, std::string& out, const std::string& in) {
    if (!connector_http_client)
        return -1;

    return handle_request(connector_http_client->Post(endpoint, in, "application/json"), out);
}

int connector::http::patch_request(const std::string& endpoint, std::string& out, const std::string& in) {
    if (!connector_http_client)
        return -1;

    return handle_request(connector_http_client->Patch(endpoint, in, "application/json"), out);
}

int connector::http::delete_request(const std::string& endpoint, std::string& out, const std::string& in) {
    if (!connector_http_client)
        return -1;

    return handle_request(connector_http_client->Delete(endpoint, in, "application/json"), out);
}

std::unique_ptr<connector::websocket::client> connector_ws_client = nullptr;

struct api_result_t {
    nlohmann::json data;
    std::string eventType;
    std::string uri;
}; NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(api_result_t, data, eventType, uri);

void connector::websocket::client::set_lock_file(const lock_file_t& lock_file) {
    m_lock_file = lock_file;
}

void connector::websocket::client::connect() {
    m_has_thread = true;
    m_thread = std::thread([this]() {
        m_client.set_access_channels(websocketpp::log::alevel::none);
        m_client.clear_access_channels(websocketpp::log::alevel::none);
        m_client.set_error_channels(websocketpp::log::elevel::none);

        m_client.init_asio();

        m_client.set_open_handler([&, this](websocketpp::connection_hdl hdl) {
            m_client.get_con_from_hdl(hdl)->send("[5,\"OnJsonApiEvent\"]");
            m_connect_handler();
        });

        m_client.set_close_handler([&, this](websocketpp::connection_hdl) {
            m_disconnect_handler();
        });

        m_client.set_tls_init_handler([](websocketpp::connection_hdl) {
            using boost::asio::ssl::context;
            return websocketpp::lib::make_shared<context>(context::tlsv12_client);
        });

        m_client.set_message_handler([this](websocketpp::connection_hdl, wsclient_t::message_ptr msg) {
            auto obj = nlohmann::json::parse(msg->get_payload(), nullptr, false);

            if (!obj.is_discarded()) {
                auto result = obj.at(2).get<api_result_t>();
                m_message_handler(result.uri, result.data.dump());
            }
        });

        m_client.set_open_handshake_timeout(900);
        m_client.set_close_handshake_timeout(900);
        m_client.set_pong_timeout(900);

        websocketpp::lib::error_code ec;
        m_conptr = m_client.get_connection("ws://127.0.0.1:" + m_lock_file.port, ec);

        if (ec)
            return;

        m_conptr->append_header("Authorization", "Basic " + lock_file_to_login_b64(m_lock_file));
        m_conptr->set_open_handshake_timeout(900);
        m_conptr->set_close_handshake_timeout(900);
        m_conptr->set_pong_timeout(900);
        
        try {
            m_client.connect(m_conptr);
            m_client.run();
        } catch(const websocketpp::exception&) {}
    });
}

void connector::websocket::client::disconnect() {
    if (m_has_thread) {
        m_has_thread = false;

        websocketpp::lib::error_code ec;
        m_client.close(m_conptr->get_handle(), websocketpp::close::status::going_away, "", ec);

        m_thread.join();
    }
}

void connector::websocket::create_and_start(const basic_handler_t& connect_handler, const basic_handler_t& disconnect_handler, const event_handler_t& message_handler, const lock_file_t& lock_file) {
    if (connector_ws_client) {
        connector_ws_client->disconnect();
        connector_ws_client = nullptr;
    }

    connector_ws_client = std::make_unique<client>(connect_handler, disconnect_handler, message_handler);
    connector_ws_client->set_lock_file(lock_file);
    connector_ws_client->connect();
}

void connector::websocket::destroy() {
    if (connector_ws_client)
        connector_ws_client->disconnect();

    connector_ws_client = nullptr;
}