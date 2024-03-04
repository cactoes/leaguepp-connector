## league++connector
a league of legends client connector written in c++20.

this library makes use of [nlohmann/json](https://github.com/nlohmann/json) for the json side of things.

> [!NOTE]
> windows only since league is windows only.

## examples
here is an full [example](example/src/main.cpp) of the library.

### connecting to the client
the header file exposes a function to connect to the client which takes in a config.
```cpp
#include <connector.hpp>
//...

// setup the config
connector::config_t config = {};
// enabling this will enable debugging for WebSocket++
config.enableWebSocketLogging = false;
// set function pointers to handlers for connect & disconnect events
config.connectHandler = &ConnectHandler;
config.disconnectHandler = &DisconnectHandler;
// after how many ms should the connector try again to connect
config.reconnectInterval = 1000;

// connect to the client using our config
connector::Connect(config);
```

### adding event handlers
to listen to events you'll need to pass the endpoint uri & an handler.
```cpp
#include <connector.hpp>
// ...

// this is what a handler looks like. always void(json)
void OnClientEvent(nlohmann::json data) { /* ... */ }

connector::AddEventHandler("/event/you-want-to-listen/to", &OnClientEvent)
```
while this list is outdated a lot of events are listed [here](https://lcu.vivide.re/) (for Client Version: 8.24)

### making requests to the client
```cpp
#include <connector.hpp>
// ...

auto result = connector::MakeRequest(connector::request_type::POST, "/uri", "\"EXTRA_JSON_DATA_HERE (if needed)\"");
// result.data will hold a json object of the data
// result.status will hold the http status code of the request
if (result.status != 200) {
    std::cout << "request failed\n";
}
```

## including
```cmake
# ================
# library: OpenSSL (v3)
# note: required for cpp-httplib
# ================
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL)

# ================
# library: Boost
# note: required for WebSocket++
# ================
set(Boost_USE_STATIC_LIBS TRUE)
add_definitions("-D_WEBSOCKETPP_CPP11_STL_")
find_package(Boost REQUIRED COMPONENTS system)
target_include_directories(${PROJECT_NAME} PRIVATE Boost::headers)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)

# ================
# library: league++connector
# ================
target_link_libraries(${PROJECT_NAME} PRIVATE leaguepp_connector)
```

## prerequisites
these are required development packages that need to be properly installed on your system.
* `openssl` (v3) (https://www.openssl.org/)
* `boost` (> v1.84.0) (https://www.boost.org/)

## notes
this was tested & built using:
* Microsoft Visual C++ 2022 / Build Tools x64
* Windows 10

## License
[MIT](LICENSE)
