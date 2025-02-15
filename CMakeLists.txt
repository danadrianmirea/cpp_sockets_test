
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

cmake_minimum_required(VERSION  3.28)

project(cpp-sockets-test)

if(WIN32)
    add_definitions(-D WINDOWS_BUILD)
endif()

add_subdirectory(code)
