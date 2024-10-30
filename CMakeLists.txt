cmake_minimum_required(VERSION 3.20)
project(leaguepp_connector)

# ~~ c++ config stuff

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ~~ global cmake vars

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

option(BUILD_EXAMPLE "build example" ON)

# -- compiler stuff

if (MSVC)
    set(CUSTOM_COMPILE_OPTIONS
        /W4 /D "_MBCS" /MP /Zc:__cplusplus /bigobj)
else()
    set(CUSTOM_COMPILE_OPTIONS
        -Wall -Wextra -D "_MBCS")
endif()

# ~~ projects

add_subdirectory(src/connector)

if (BUILD_EXAMPLE)
    add_subdirectory(src/example)
endif()