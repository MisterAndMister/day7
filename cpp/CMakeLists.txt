cmake_minimum_required(VERSION 3.10)
project(sysmon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_library(sysmon SHARED
    data_collector.cpp
)


target_include_directories(sysmon
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(sysmon PROPERTIES
    OUTPUT_NAME sysmon
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
)

set_target_properties(sysmon PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

find_package(Python3 COMPONENTS Development REQUIRED)
target_include_directories(sysmon PRIVATE ${Python3_INCLUDE_DIRS})
