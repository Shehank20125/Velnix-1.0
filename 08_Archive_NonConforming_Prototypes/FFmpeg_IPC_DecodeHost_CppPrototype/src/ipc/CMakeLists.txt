add_library(mpe_ipc INTERFACE)
target_include_directories(mpe_ipc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mpe_ipc INTERFACE mpe_common)

# When a real transport is chosen (named pipes/sockets vs gRPC), convert
# this to a regular add_library() target with the corresponding .cpp,
# and link its transport-specific deps (e.g. gRPC::grpc++) here.
