set(BUILD_NAME "log_test")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LISTS)
add_executable(${BUILD_NAME} ${SRC_LISTS})
