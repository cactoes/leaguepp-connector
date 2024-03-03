#ifndef __WSCLIENT_HPP__
#define __WSCLIENT_HPP__

#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4267)
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#pragma warning(pop)

#include <functional>
#include <nlohmann/json.hpp>
#include <string>

#include "lockfile.hpp"

namespace wsclient {
    typedef websocketpp::client<websocketpp::config::asio_tls_client> wsclient_t;
    typedef websocketpp::client<websocketpp::config::asio_tls_client>::connection_ptr wsconnection_ptr;
    typedef boost::asio::ssl::context boost_asio_ctx;

    typedef std::function<void(void)> connect_handler_t;
    typedef std::function<void(void)> disconnect_handler_t;
    typedef std::function<void(std::string, nlohmann::json)> message_handler_t;

    struct api_result_t {
        nlohmann::json data;
        std::string eventType;
        std::string uri;
    }; NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(api_result_t, data, eventType, uri);

    struct client_handlers_t {
        connect_handler_t connect;
        disconnect_handler_t disconnect;
        message_handler_t message;
    };

    bool HasValidHandlers(client_handlers_t* handlers);

    class client {
    public:
        client() = default;

        void SetLockFile(riot::lock_file_t* lockFile);
        void Connect();
        void Disconnect();

    private:
        void InternalRunner();

    private:
        wsclient_t m_client;
        wsconnection_ptr m_connection;

        std::thread m_thread;
        bool m_hasThread = false;

        riot::lock_file_t* m_lockFile;
    };

    void SetWSLoggingState(bool state);

    void SetConnectHandler(connect_handler_t handler);
    void SetDisconnectHandler(disconnect_handler_t handler);
    void SetMessageHandler(message_handler_t handler);

    bool CreateAndStart(riot::lock_file_t* lockFile);
    void Destroy();
}; // namespace wsclient

#endif // __WSCLIENT_HPP__