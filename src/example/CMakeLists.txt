cmake_minimum_required(VERSION 3.20)
project(example)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
    PRIVATE ${connector_headers})

target_link_libraries(${PROJECT_NAME}
    PRIVATE ${connector_lib})

target_compile_options(${PROJECT_NAME} PRIVATE ${CUSTOM_COMPILE_OPTIONS})