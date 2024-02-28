#include "client.hpp"

client::WebSocketClientT::connection_ptr g_pConnection;
client::WebSocketClientT g_client;
LockFileT g_lockfile;

bool client::Init(const LockFileT& lockFile) {
    g_lockfile = lockFile;

    // try {
#ifndef _DEBUG
    g_client.set_access_channels(websocketpp::log::alevel::none);
    g_client.clear_access_channels(websocketpp::log::alevel::none);
    g_client.set_error_channels(websocketpp::log::elevel::none);
#else
    g_client.set_access_channels(websocketpp::log::alevel::all);
    g_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
    g_client.set_error_channels(websocketpp::log::elevel::all);
#endif

    // listen to events
    g_client.set_open_handler([](websocketpp::connection_hdl) { g_pConnection->send("[5,\"OnJsonApiEvent\"]"); });

    g_client.set_message_handler(&OnMessage);

    g_client.set_tls_init_handler([](websocketpp::connection_hdl) {
        return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12_client);
    });

    websocketpp::lib::error_code ec;
    g_pConnection = g_client.get_connection("ws://127.0.0.1:" + g_lockfile.m_port, ec);
    g_pConnection->append_header("Authorization", "Basic " + g_lockfile.AsBase64());

    if (ec) {
#ifdef _DEBUG
        std::cout << "could not create connection because: " << ec.message() << std::endl;
#endif
        return false;
    }

    g_client.connect(g_pConnection);
    g_client.run();

    // } catch (websocketpp::exception const & e) {
    //     std::cout << e.what() << std::endl;
    // }

    return true;
}