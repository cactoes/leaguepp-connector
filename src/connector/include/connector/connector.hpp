#pragma once

#ifndef __CONNECTOR_CONNECTOR_HPP__
#define __CONNECTOR_CONNECTOR_HPP__

#include <functional>
#include <string>
#include <map>
#include <optional>
#include <vector>
#include <thread>

namespace connector {
    struct settings_t {
        int reconnect_interval = 1000;
    };

    enum request_type_t {
        RT_GET = 0,
        RT_PUT,
        RT_POST,
        RT_PATCH,
        RT_DEL
    };

    typedef std::function<void(void)> basic_handler_t;
    typedef std::function<void(std::string, std::string)> event_handler_t;

    struct result_t {
        int status_code;
        std::string data;
    };

    class connector {
    public:
        connector(settings_t settings = {}) : m_settings(settings) {}

        void set_connect_handler(const basic_handler_t& handler);
        void set_disconnect_handler(const basic_handler_t& handler);

        void connect();
        void disconnect();
        bool is_connected() const;

        result_t make_request(request_type_t type, const std::string& endpoint, const std::string& data = "");
        std::optional<std::string> make_request_data(request_type_t type, const std::string& endpoint, const std::string& data = "", int status = 200);

        void add_event_listener(const std::string& endpoint, const event_handler_t& handler);
    
    private:
        bool m_is_connected = false;
        bool m_keep_alive = false;
        settings_t m_settings;
        std::map<std::string, std::vector<event_handler_t>> m_event_handlers {};
        std::optional<basic_handler_t> m_connect_handler = std::nullopt;
        std::optional<basic_handler_t> m_disconnect_handler = std::nullopt;
        std::thread m_active_thread;
    };
} // namespace connector

#endif // __CONNECTOR_CONNECTOR_HPP__