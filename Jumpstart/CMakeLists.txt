cmake_minimum_required(VERSION 3.15)
project(Jumpstart VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    Jumpstart/jumpstart.cpp
    Jumpstart/app-router.cpp
    Jumpstart/router.cpp
    Jumpstart/init-controller.cpp
    Jumpstart/tcp_ip_listener.cpp
    Jumpstart/request_handler.cpp
    Jumpstart/global_variables.cpp
)

# Create executable
add_executable(jumpstart ${SOURCES})

# Include directories
target_include_directories(jumpstart PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Jumpstart
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(jumpstart PRIVATE ws2_32)
    target_compile_definitions(jumpstart PRIVATE _WIN32)
else()
    target_link_libraries(jumpstart PRIVATE pthread)
    # Add other Linux libraries if needed
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(jumpstart PRIVATE /W4 /utf-8)
else()
    target_compile_options(jumpstart PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Output directory
set_target_properties(jumpstart PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Building for platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
