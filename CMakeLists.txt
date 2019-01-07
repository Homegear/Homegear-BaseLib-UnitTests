cmake_minimum_required(VERSION 3.13)
project(homegear_baselib_unittests)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/HelperFunctions.cpp
        src/HelperFunctions.h
        src/Json.cpp
        src/Json.h
        src/Math.cpp
        src/Math.h
        src/WebSocket.cpp
        src/WebSocket.h
        src/main.cpp)

add_executable(homegear_baselib_unittests ${SOURCE_FILES})

target_link_libraries(homegear_baselib_unittests homegear-base gnutls gcrypt)