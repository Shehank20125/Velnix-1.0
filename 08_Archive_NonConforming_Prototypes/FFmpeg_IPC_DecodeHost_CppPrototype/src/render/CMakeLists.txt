add_library(mpe_render INTERFACE)
target_include_directories(mpe_render INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mpe_render INTERFACE mpe_common)
