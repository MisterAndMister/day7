cmake_minimum_required(VERSION 3.10)
project(sysmon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Собираем как shared-библиотеку
add_library(sysmon SHARED data_collector.cpp)

# Для Python-биндингов (опционально)
find_package(Python3 COMPONENTS Development REQUIRED)
target_include_directories(sysmon PRIVATE ${Python3_INCLUDE_DIRS})