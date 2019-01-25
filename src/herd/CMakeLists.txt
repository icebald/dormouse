cmake_minimum_required(VERSION 3.0.0)

#project(herd VERSION 0.1 LANGUAGES CXX)
set(PROJECT_NAME herd)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH /home/icebear/devtools/qt/5.12.0/gcc_64)

find_package(Qt5 REQUIRED Widgets)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/dev SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/adb SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ui SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST} ui/events.h)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
