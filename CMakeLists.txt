cmake_minimum_required(VERSION 3.16)
project(PoliticalSim)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(src)

add_executable(PoliticalSim main.cpp)

target_include_directories(PoliticalSim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(PoliticalSim
    Qt6::Core Qt6::Widgets
    src_lib
)
