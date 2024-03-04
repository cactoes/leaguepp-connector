cmake_minimum_required(VERSION 3.20)
project(leaguepp_connector)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ================
# main library
# ================
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*)
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} INTERFACE src/include)

# ================
# submodule: cpp-httplib
# ================
target_include_directories(${PROJECT_NAME} INTERFACE lib/cpp-httplib)

# ================
# submodule: WebSocket++
# ================
target_include_directories(${PROJECT_NAME} INTERFACE lib/websocketpp)

# ================
# submodule: json
# ================
target_include_directories(${PROJECT_NAME} INTERFACE lib/json/single_include)

# enable to build the example
# add_subdirectory(example)