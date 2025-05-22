cmake_minimum_required(VERSION 3.16)
project(PoliticalSim)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 COMPONENTS Core REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(PoliticalSim
    main.cpp
)

target_link_libraries(PoliticalSim Qt6::Core)