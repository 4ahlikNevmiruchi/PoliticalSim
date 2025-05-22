set(GUI_SRC
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
)

add_library(gui_lib ${GUI_SRC})

target_include_directories(gui_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gui_lib Qt6::Widgets)