
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)

# 去除文件名路径
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst $(dir $<),,$<)\"'")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/os)
if (APPLE)
    set(OS_NAME "mac")
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(OS_NAME "linux")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/os/${OS_NAME})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/herd)
