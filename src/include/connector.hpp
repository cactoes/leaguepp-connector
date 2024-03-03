#ifndef __CONNECTOR_HPP__
#define __CONNECTOR_HPP__

#include <memory>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>

namespace connector {
    enum connection_state {
        NONE = 0,
        DISCONNECTED,
        NOCLIENT,
        READY,
        CONNECTED
    };

    struct config_t {
        bool enableWebSocketLogging = false;
        std::function<void(void)> connectHandler = nullptr;
        std::function<void(void)> disconnectHandler = nullptr;
    };

    // non-blocking. setup the connection to the client.
    // this function will block until the client is opened
    void Connect(const config_t& config);

    // blocking. close the connector
    void Disconnect();

    // std::bind(&CLASSNAME::FUNCTION, &INSTANCE, std::placeholder::_N)
    void AddEventHandler(const std::string& endpoint, std::function<void(nlohmann::json)> listener);
}; // namespace connector

#endif // __CONNECTOR_HPP__