SET(EXAMPLE_PROJECT_NAME example)

# ================
# main
# ================
file(GLOB_RECURSE EXAMPLE_SOURCE_FILES CONFIGURE_DEPENDS src/*)
add_executable(${EXAMPLE_PROJECT_NAME} ${EXAMPLE_SOURCE_FILES})

# ================
# library: OpenSSL (v3)
# ================
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
target_link_libraries(${EXAMPLE_PROJECT_NAME} PRIVATE OpenSSL::SSL)

# ================
# library: Boost
# note: required for WebSocket++
# ================
set(Boost_USE_STATIC_LIBS TRUE)
add_definitions("-D_WEBSOCKETPP_CPP11_STL_")
find_package(Boost REQUIRED COMPONENTS filesystem system)
target_include_directories(${EXAMPLE_PROJECT_NAME} PRIVATE Boost::headers)
target_link_libraries(${EXAMPLE_PROJECT_NAME} PRIVATE Boost::system)

# ================
# library: league++connector
# note: this is not the same way it should be link in a real application (see readme)
# ================
target_link_libraries(${EXAMPLE_PROJECT_NAME} PRIVATE ${PROJECT_NAME})

if(MSVC)
    target_compile_options(${EXAMPLE_PROJECT_NAME} PRIVATE /W4 /D "_MBCS")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oxs")
    set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++20")
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()