cmake_minimum_required(VERSION 3.16)
project(chatdemo C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(chat_core
    src/platform.c
    src/socketx.c
    src/threadx.c
    src/utf8.c
    src/chat_protocol.c
    src/chat_server.c
    src/chat_client.c
)

target_include_directories(chat_core PUBLIC inc)

if(WIN32)
    target_link_libraries(chat_core PUBLIC ws2_32 shell32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(chat_core PUBLIC Threads::Threads)
endif()

add_executable(chat_server src/server_main.c)
target_link_libraries(chat_server PRIVATE chat_core)

add_executable(chat_client src/client_main.c)
target_link_libraries(chat_client PRIVATE chat_core)

