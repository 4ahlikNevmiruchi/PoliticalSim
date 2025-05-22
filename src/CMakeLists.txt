add_subdirectory(gui)

add_library(src_lib
    #links gui_lib
)

target_link_libraries(src_lib
    gui_lib    
)
