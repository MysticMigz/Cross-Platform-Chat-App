cmake_minimum_required(VERSION 3.12)
project(ChatApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    set(PLATFORM_LIBS ws2_32)
else()
    set(PLATFORM_LIBS pthread)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/shared)

# Server executable
add_executable(chat-server
    server/main.cpp
    server/Server.cpp
    server/Server.h
    server/ClientSession.cpp
    server/ClientSession.h
    server/MessageRouter.cpp
    server/MessageRouter.h
    server/Protocol.cpp
    server/Protocol.h
)

target_link_libraries(chat-server ${PLATFORM_LIBS})

# Client executable
add_executable(chat-client
    client/main.cpp
    client/Client.cpp
    client/Client.h
    client/Network.cpp
    client/Network.h
    client/UI.cpp
    client/UI.h
)

target_link_libraries(chat-client ${PLATFORM_LIBS})

# Set output directories
set_target_properties(chat-server chat-client
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS chat-server chat-client
    RUNTIME DESTINATION bin
)

