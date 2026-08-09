add_executable(mpe_decode_host
    main.cpp
)

target_link_libraries(mpe_decode_host PRIVATE
    mpe_common
    mpe_ipc
    mpe_decode
)

# This is the ONLY target permitted to link mpe_decode (and therefore
# FFmpeg). Keep it that way — see src/decode/CMakeLists.txt.
