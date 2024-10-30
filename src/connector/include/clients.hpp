#pragma once

#ifndef __CONNECTOR_CLIENTS_HPP__
#define __CONNECTOR_CLIENTS_HPP__

#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4267)
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#pragma warning(pop)

#include <string>
#include <thread>

#include "lockfile.hpp"

namespace connector {
    namespace http {
        void create(const lock_file_t& lock_file);
        void destroy();
        
        int get_request(const std::string& endpoint, std::string& out);
        int put_request(const std::string& endpoint, std::string& out, const std::string& in = "");
        int post_request(const std::string& endpoint, std::string& out, const std::string& in = "");
        int patch_request(const std::string& endpoint, std::string& out, const std::string& in = "");
        int delete_request(const std::string& endpoint, std::string& out, const std::string& in = "");
    } // namespace http

    namespace websocket {
        typedef std::function<void(void)> basic_handler_t;
        typedef std::function<void(std::string, std::string)> event_handler_t;

        typedef websocketpp::client<websocketpp::config::asio_tls_client> wsclient_t;
        typedef websocketpp::client<websocketpp::config::asio_tls_client>::connection_ptr wsconnection_ptr_t;
        typedef boost::asio::ssl::context boost_asio_ctx_t;

        class client {
        public:
            client(const basic_handler_t& connect_handler, const basic_handler_t& disconnect_handler,  const event_handler_t& message_handler)
                : m_connect_handler(connect_handler), m_disconnect_handler(disconnect_handler), m_message_handler(message_handler) {};

            void set_lock_file(const lock_file_t& lock_file);

            void connect();
            void disconnect();

        private:
            wsclient_t m_client;
            wsconnection_ptr_t m_conptr;

            basic_handler_t m_connect_handler;
            basic_handler_t m_disconnect_handler;
            event_handler_t m_message_handler;

            lock_file_t m_lock_file;

            void* m_client_ptr_raw = nullptr;
            void* m_connection_ptr_raw = nullptr;

            std::thread m_thread;
            bool m_has_thread = false;
        };

        void create_and_start(const basic_handler_t& connect_handler, const basic_handler_t& disconnect_handler, const event_handler_t& message_handler, const lock_file_t& lock_file);
        void destroy();
    } // namespace websocket
} // namespace connector

#endif // __CONNECTOR_CLIENTS_HPP__