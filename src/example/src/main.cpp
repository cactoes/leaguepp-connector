#define WIN32_LEAN_AND_MEAN

#include <connector.hpp>
#include <iostream>
#include <thread>
#include <Windows.h>

int main() {
    connector::settings_t settings {};
    settings.reconnect_interval = 1000;

    connector::connector client(settings);

    client.set_connect_handler([]() {
        std::cout << "connected\n";
    });    

    client.set_disconnect_handler([]() {
        std::cout << "disconnected\n";
    });

    client.add_event_listener("/lol-gameflow/v1/gameflow-phase", [](std::string, std::string data) {
        std::cout << data << "\n";
    });

    client.connect();

    while (!GetAsyncKeyState(VK_END)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    client.disconnect();

    return 0;
}