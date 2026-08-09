add_library(mpe_decode STATIC
    HwAccelDetector.cpp
    Decoder.cpp
)
target_include_directories(mpe_decode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mpe_decode
    PUBLIC mpe_common
    PRIVATE PkgConfig::FFMPEG
)

# IMPORTANT: this library must only ever be linked into decode_host, never
# into app — that's the whole point of the decode-isolation design
# (Threat Model item 1). Do not add mpe_decode as a dependency of mpe_app.
