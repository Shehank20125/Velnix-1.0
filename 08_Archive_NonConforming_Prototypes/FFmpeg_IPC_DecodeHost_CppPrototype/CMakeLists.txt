cmake_minimum_required(VERSION 3.20)
project(MediaPlayerCoreEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build unit tests" OFF)

# ---------------------------------------------------------------------------
# Third-party dependencies
# ---------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
)
# NOTE: On Windows, prefer vcpkg's ffmpeg port over pkg-config; see
# cmake/FindFFmpeg.cmake for the fallback path if pkg-config isn't available.

# ---------------------------------------------------------------------------
# Subsystems
#   common      - shared POD types (frame descriptors, codec enums) used by
#                 both the main app and the Decode Host process.
#   ipc         - transport between main app <-> Decode Host. STUBBED:
#                 no wire protocol chosen yet, see src/ipc/README.md.
#   decode      - FFmpeg wrapper + hw-accel auto-detection. Runs *inside*
#                 the Decode Host process, never in the main app process.
#   render      - frame sink interface consumed by the Qt UI to paint
#                 decoded frames (shared-texture path is stubbed).
#   app         - main Qt application (UI, playback control, orchestration).
#   decode_host - isolated process entry point; links decode + ipc, NOT app.
# ---------------------------------------------------------------------------
add_subdirectory(src/common)
add_subdirectory(src/ipc)
add_subdirectory(src/decode)
add_subdirectory(src/render)
add_subdirectory(src/app)
add_subdirectory(src/decode_host)

# Dev/test tool: decodes a real media file end-to-end through mpe_decode
# without needing the Qt app or IPC transport. Useful for validating
# Decoder/HwAccelDetector changes in isolation. Not part of the shipped
# product.
add_subdirectory(tools/decode_probe)

if(BUILD_TESTS)
    enable_testing()
    # add_subdirectory(tests)   # not scaffolded yet
endif()
