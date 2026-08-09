qt_standard_project_setup()

add_executable(mpe_app
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

target_link_libraries(mpe_app PRIVATE
    Qt6::Widgets
    mpe_common
    mpe_ipc
    mpe_render
)

# Deliberately NOT linked here: mpe_decode. This process must never link
# FFmpeg directly — decoding only happens in decode_host (see
# src/decode/CMakeLists.txt for the enforcement note).
