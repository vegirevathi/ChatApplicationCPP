cmake_minimum_required(VERSION 3.16.3)

project(Server)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(executableServer ../controller/serverController.cpp ../model/server.cpp ../model/server.h)

find_package(Threads REQUIRED)

target_link_libraries(executableServer PRIVATE Threads::Threads)