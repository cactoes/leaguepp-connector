#include "connector/connector.hpp"
#include "clients.hpp"

void connector::connector::set_connect_handler(const basic_handler_t& handler) {
    m_connect_handler = handler;
}

void connector::connector::set_disconnect_handler(const basic_handler_t& handler) {
    m_disconnect_handler = handler;
}

void connector::connector::connect() {
    if (!m_keep_alive) {
        m_keep_alive = true;
        m_active_thread = std::thread([this]() {
            lock_file_t lock_file {};

            auto connect_handler = [this]() {
                m_is_connected = true;
                if (m_connect_handler.has_value())
                    m_connect_handler.value()();
            };

            auto disconnect_handler = [&, this]() {
                m_is_connected = false;
                lock_file = {};
                if (m_disconnect_handler.has_value())
                    m_disconnect_handler.value()();
            };

            auto message_handler = [this](std::string uri, std::string data) {
                if (m_event_handlers.contains("*"))
                    for (const auto& handler : m_event_handlers.at("*"))
                        handler(uri, data);

                if (m_event_handlers.contains(uri))
                    for (const auto& handler : m_event_handlers.at(uri))
                        handler(uri, data);
            };

            while (m_keep_alive) {
                if (m_is_connected) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(m_settings.reconnect_interval));
                    continue;
                }

                if (!lock_file_validate(lock_file)) {
                    auto lock_file_result = lock_file_get();
                    if (!lock_file_result.has_value()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(m_settings.reconnect_interval));
                        continue;
                    }

                    lock_file = lock_file_result.value();
                    http::create(lock_file);
                }

                std::string tmp;
                if (http::get_request("/lol-gameflow/v1/gameflow-phase", tmp) == 200)
                    websocket::create_and_start(connect_handler, disconnect_handler, message_handler, lock_file);

                std::this_thread::sleep_for(std::chrono::milliseconds(m_settings.reconnect_interval));
            }

            http::destroy();
            websocket::destroy();
        });
    }
}

void connector::connector::disconnect() {
    if (m_keep_alive) {
        m_keep_alive = false;
        m_active_thread.join();
    }
}

bool connector::connector::is_connected() const {
    return m_is_connected;
}

connector::result_t connector::connector::make_request(request_type_t type, const std::string& endpoint, const std::string& data) {
    result_t result = {};

    switch (type) {
        case request_type_t::RT_GET:
            result.status_code = http::get_request(endpoint, result.data);
            break;
        case request_type_t::RT_PUT:
            result.status_code = http::put_request(endpoint, result.data, data);
            break;
        case request_type_t::RT_POST:
            result.status_code = http::post_request(endpoint, result.data, data);
            break;
        case request_type_t::RT_PATCH:
            result.status_code = http::patch_request(endpoint, result.data, data);
            break;
        case request_type_t::RT_DEL:
            result.status_code = http::delete_request(endpoint, result.data, data);
            break;
        default:
            result.status_code = -1;
            break;
    }

    return result;
}

std::optional<std::string> connector::connector::make_request_data(request_type_t type, const std::string& endpoint, const std::string& data, int status) {
    if (auto result = make_request(type, endpoint, data); result.status_code == status)
        return result.data;

    return std::nullopt;
}

void connector::connector::add_event_listener(const std::string& endpoint, const event_handler_t& handler) {
    m_event_handlers[endpoint].push_back(handler);
}