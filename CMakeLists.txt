cmake_minimum_required(VERSION 3.16)
project(PoliticalSim)

set(CMAKE_CXX_STANDARD 17)

# Qt (опціонально — поки без GUI)
find_package(Qt5 COMPONENTS Core REQUIRED)

# Catch2
include(cmake/Catch2.cmake)

# Source
add_library(PoliticalSimLib
    src/models/Party.cpp
)

# Main
add_executable(PoliticalSim main.cpp)
target_link_libraries(PoliticalSim PoliticalSimLib Qt5::Core)

# Tests
add_executable(UnitTests tests/test_party.cpp)
target_link_libraries(UnitTests PoliticalSimLib Catch2::Catch2WithMain Qt5::Core)