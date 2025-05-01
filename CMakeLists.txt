cmake_minimum_required(VERSION 3.21)
project(networking)

include_directories(include)

add_executable(server 
    src/httpserver.cpp
    main/server_main.cpp)