cmake_minimum_required(VERSION 3.20)
project(connector)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set(${PROJECT_NAME}_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/connector PARENT_SCOPE)
set(${PROJECT_NAME}_lib ${PROJECT_NAME} PARENT_SCOPE)

target_include_directories(${PROJECT_NAME}
    PRIVATE include
    PRIVATE ${LIB_DIR}/cpp-httplib
    PRIVATE ${LIB_DIR}/websocketpp
    PRIVATE ${LIB_DIR}/json/single_include)

# target_link_libraries(${PROJECT_NAME})

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

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_compile_options(${PROJECT_NAME} PRIVATE ${CUSTOM_COMPILE_OPTIONS})