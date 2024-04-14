#ifndef __CONNECTOR_HPP__
#define __CONNECTOR_HPP__

#include <memory>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>

namespace connector {
    struct config_t {
        bool enableWebSocketLogging = false;
        std::function<void(void)> connectHandler = nullptr;
        std::function<void(void)> disconnectHandler = nullptr;
        int reconnectInterval = 1000;
    };

    enum class request_type {
        GET = 0,
        PUT,
        POST,
        PATCH,
        DEL
    };

    struct result_t {
        int status;
        nlohmann::json data;
    };

    // non-blocking. setup the connection to the client.
    // this function will block until the client is opened
    void Connect(const config_t& config);

    // blocking. close the connector
    void Disconnect();

    // std::bind(&CLASSNAME::FUNCTION, &INSTANCE, std::placeholder::_N)
    void AddEventHandler(const std::string& endpoint, std::function<void(std::string, nlohmann::json)> listener);

    // data is optional and only used when sending data to the client
    result_t MakeRequest(request_type type, const std::string& endpoint, const std::string& data = "");
}; // namespace connector

#endif // __CONNECTOR_HPP__