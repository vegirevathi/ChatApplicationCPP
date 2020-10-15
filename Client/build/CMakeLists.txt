cmake_minimum_required(VERSION 3.16.3)

project(Client)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(executableClient ../controller/clientController.cpp ../controller/clientController.h
                                 ../model/client.cpp ../model/client.h)

find_package(Threads REQUIRED)

target_link_libraries(executableClient PRIVATE Threads::Threads)