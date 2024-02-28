#ifndef __CLIENT__HPP__
#define __CLIENT__HPP__

#pragma warning(push)
#pragma warning( disable: 4127 )
#pragma warning( disable: 4267 )
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#pragma warning(pop)

#include <string>
// #include <iostream>
#include <vector>
#include <unordered_map>

#include "lockfile.hpp"

namespace client {
    typedef websocketpp::client<websocketpp::config::asio_tls_client> WebSocketClientT;

    extern WebSocketClientT::connection_ptr g_pConnection;
    extern WebSocketClientT g_client;
    extern LockFileT g_lockfile;

    bool Init(const LockFileT& lockFile);
    
    static void OnMessage(websocketpp::connection_hdl, WebSocketClientT::message_ptr msg) {
        // std::cout << msg->get_payload() << std::endl;
    }
}; // namespace client

#endif // __CLIENT__HPP__