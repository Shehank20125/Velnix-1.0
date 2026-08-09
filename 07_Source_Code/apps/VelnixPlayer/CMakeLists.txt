# The GUI application: Qt6/QML front-end + libmpv playback backend,
# built on top of the VelnixCore orchestration library.

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick OpenGL)
qt_standard_project_setup()

# libmpv lives in third_party/libmpv — drop the mpv-dev package there
# (mpv.dll, mpv.lib, include/) so the build is self-contained and doesn't
# depend on an absolute path on your machine. See third_party/libmpv/NOTES.md.
set(LIBMPV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/libmpv)

qt_add_executable(VelnixPlayer
    src/main.cpp
    src/MpvObject.cpp
    src/MpvObject.h
    src/EngineController.cpp
    src/EngineController.h
)

qt_add_qml_module(VelnixPlayer
    URI VelnixPlayer
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

target_include_directories(VelnixPlayer PRIVATE
    ${LIBMPV_DIR}/include
)

target_link_directories(VelnixPlayer PRIVATE
    ${LIBMPV_DIR}
)

target_link_libraries(VelnixPlayer PRIVATE
    VelnixCore
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::OpenGL
    mpv
)

# libmpv ships a .dll but MSVC needs a .lib import library to link against.
# If mpv.lib isn't included in your download, generate one from mpv-1.def:
#   lib /def:mpv-1.def /out:mpv.lib /machine:x64
# (run from an "x64 Native Tools Command Prompt for VS 2022")
