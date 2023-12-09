configure_file(patchman_config.h.in ${PROJECT_BINARY_DIR}/include/patchman_config.h)
include_directories(${PROJECT_BINARY_DIR}/include)

add_subdirectory(patchlib)
add_subdirectory(patchman)
