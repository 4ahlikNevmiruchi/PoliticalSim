cmake_minimum_required(VERSION 3.16)
project(PoliticalSim)

set(CMAKE_CXX_STANDARD 17)


find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
)

add_executable(PoliticalSim
    main.cpp

    src/gui/MainWindow.cpp
    src/gui/MainWindow.h
    src/gui/MainWindow.ui

    src/gui/AddPartyDialog.cpp
    src/gui/AddPartyDialog.h
    src/gui/AddPartyDialog.ui

    src/models/PartyModel.cpp
    src/models/PartyModel.h
    
)

target_link_libraries(PoliticalSim
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)