# Builds velnix::core as a static library — the orchestration layer
# (MediaEngine, SessionManager, ThreadManager, EventDispatcher, CommandQueue,
# Logger). Framework-agnostic C++20, no Qt/mpv dependency by design, so it
# can sit underneath any UI or playback backend.

file(GLOB_RECURSE VELNIX_CORE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
file(GLOB_RECURSE VELNIX_CORE_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_library(VelnixCore STATIC
    ${VELNIX_CORE_SOURCES}
    ${VELNIX_CORE_HEADERS}
)

target_include_directories(VelnixCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(VelnixCore PUBLIC cxx_std_20)
