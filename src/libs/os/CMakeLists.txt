
set(OS_NAME "")
if (APPLE)
    set(OS_NAME "mac")
endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${OS_NAME})