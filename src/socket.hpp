#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#pragma warning(push)
#pragma warning( disable: 4127 )
#pragma warning( disable: 4267 )
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#pragma warning(pop)

#include <string>
#include <iostream>
#include <vector>

std::string base64_encode(const std::string& input) {
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (encoded.size() % 4) encoded.push_back('=');
    return encoded;
}

namespace socket123 {
    typedef websocketpp::client<websocketpp::config::asio_tls_client> WebSocketClientT;

    static void OnMessage(websocketpp::connection_hdl, WebSocketClientT::message_ptr msg) {
        std::cout << msg->get_payload() << std::endl;
    }

    static void Test(const std::string& url) {
        std::cout << "creating connection: " << url << "\n";

        WebSocketClientT client;
        
        try {
            client.set_access_channels(websocketpp::log::alevel::all);
            client.clear_access_channels(websocketpp::log::alevel::frame_payload);
            client.set_error_channels(websocketpp::log::elevel::all);

            client.init_asio();

            WebSocketClientT::connection_ptr con;

            client.set_open_handler([&con](websocketpp::connection_hdl) {
                con->send("[5,\"OnJsonApiEvent\"]");
            });

            client.set_message_handler(&OnMessage);

            client.set_tls_init_handler([](websocketpp::connection_hdl) {
                return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12_client);
            });

            websocketpp::lib::error_code ec;
            con = client.get_connection(url, ec);
            con->append_header("Authorization", "Basic " + base64_encode("riot:ZkpiUl-gH9LkLqUuigvyUQ"));

            if (ec) {
                std::cout << "could not create connection because: " << ec.message() << std::endl;
                return;
            }

            client.connect(con);

            client.run();
        } catch (websocketpp::exception const & e) {
            std::cout << e.what() << std::endl;
        }
    }
}; // namespace socket123

#endif // __SOCKET_HPP__