cmake_minimum_required(VERSION 3.21)
project(Velnix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# velnix::core — framework-agnostic orchestration layer (Engine, Session,
# Threading, Events, Commands, Logging). No Qt/mpv dependency.
add_subdirectory(src/Core)

# The GUI application (Qt6/QML + libmpv playback).
add_subdirectory(apps/VelnixPlayer)

# apps/DecodeHost, src/FFmpeg, src/Renderer, src/Hardware, src/Playback,
# src/Audio, src/Subtitle are intentionally not wired in yet — see
# apps/DecodeHost/NOTES.md. These are the seams for a future custom
# decode/render engine, once VelnixPlayer is shipping on libmpv.
