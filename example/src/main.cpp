#include <Windows.h>

#include <connector.hpp>

#include <iostream>
#include <thread>

void ConnectHandler() {
    std::cout << "on connect event\n";
}

void DisconnectHandler() {
    std::cout << "on disconnect event\n";
}

class GameFlowHandler {
public:
    void OnNewGameFlow(nlohmann::json data) {
        std::cout << "current gameflow: " + data.get<std::string>() << "\n";
    }
};

int main() {
    connector::config_t config = {};
    config.enableWebSocketLogging = false;
    config.connectHandler = &ConnectHandler;
    config.disconnectHandler = &DisconnectHandler;

    GameFlowHandler handler = GameFlowHandler();
    connector::AddEventHandler("/lol-gameflow/v1/gameflow-phase",
        std::bind(&GameFlowHandler::OnNewGameFlow, &handler, std::placeholders::_1));

    connector::Connect(config);

    while (!GetAsyncKeyState(VK_END)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    connector::Disconnect();

    return 0;
}