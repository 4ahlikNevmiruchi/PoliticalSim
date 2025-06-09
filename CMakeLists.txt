cmake_minimum_required(VERSION 3.16)
project(PoliticalSim)

set(CMAKE_CXX_STANDARD 17)

# Find required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Charts)

# Enable Qt automoc, autouic, autorcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
)

# Catch2 setup (must come before UnitTests target)
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(Catch2)

# GUI Application Target
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
    src/models/Voter.h

    src/models/VoterModel.h
    src/models/VoterModel.cpp
    src/gui/addvoterdialog.h src/gui/addvoterdialog.cpp src/gui/addvoterdialog.ui

    src/widgets/PartyChartWidget.cpp
    src/widgets/PartyChartWidget.h
)

# Includes for GUI
target_include_directories(PoliticalSim
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
)

# Link Qt for GUI
target_link_libraries(PoliticalSim
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
)

# Unit Test Target
add_executable(UnitTests
    tests/test_party_model.cpp
    tests/test_party_db.cpp
    tests/test_voter_model.cpp
    tests/test_party_popularity.cpp

    src/models/PartyModel.cpp
    src/models/VoterModel.cpp

    src/models/PartyModel.h
    src/models/Voter.h
)

# Includes for UnitTests (including Catch2)
target_include_directories(UnitTests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${catch2_SOURCE_DIR}/src
)

# Link Qt + Catch2 for tests
target_link_libraries(UnitTests
    Qt6::Core
    Qt6::Sql
    Catch2::Catch2WithMain
)
