## league++connector
A league of legends client connector written in c++20.

> [!NOTE]
> Windows only since league is windows only.

## Examples
Here is an full [example](src/example/src/main.cpp) of the library.

### Connecting to the client
The header file exposes a function to connect to the client which takes in a config.
```cpp
#include <connector.hpp>
//...

// setup the config
connector::settings_t settings = {};
config.reconnect_interval = 1000;

// connect to the client using our config
connector::connector client(settings);
```

### Adding event handlers
to listen to events you'll need to pass the endpoint uri & an handler.
```cpp
// this is what a handler looks like. always void(string, string)
void on_client_event(std::string uri, std::string data);

client.add_event_listener("/event/you-want-to-listen/to", &on_client_event)
```
While this list is outdated a lot of events are listed [here](https://lcu.vivide.re/) (for Client Version: 8.24)

### Making requests to the client
```cpp
auto result = client.make_request(connector::request_type::POST, "/uri", "\"EXTRA_JSON_DATA_HERE (if needed)\"");
// result.data will hold a json object of the data in string format
// result.status will hold the http status code of the request
if (result.status != 200)
    std::cout << "request failed\n";
```

## Including
Import the bins in the release & include the header(s) in [connector/include/connector](connector/include/connector)

# Builing form source
To build from source make sure you have cloned this repository with `--recursive`.

## Prerequisites
These are required development packages that need to be properly installed on your system.
* `openssl` (v3) (https://www.openssl.org/)
* `boost` (> v1.84.0) (https://www.boost.org/)

# Notes
this was tested & built using:
* Microsoft Visual C++ 2022 / Build Tools x64
* Windows 10

## License
[MIT](LICENSE)
