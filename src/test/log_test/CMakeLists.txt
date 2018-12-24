set(BUILD_NAME "log_test")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LISTS)
add_executable(${BUILD_NAME} ${SRC_LISTS} ../../libs/concurrent/dispatch.h ../../libs/concurrent/message_queue.h ../../libs/concurrent/looper.h ../../libs/concurrent/handler.h ../../libs/concurrent/message.h ../../libs/net/net_dispatch.h ../../libs/utils/invoker.h)
target_link_libraries(${BUILD_NAME}
        pthread
        event)