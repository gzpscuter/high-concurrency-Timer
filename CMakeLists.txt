project(timer)

add_definitions(--std=c++17)

include_directories(${PROJECT_SOURCE_DIR} delay_queue thread_pool)
link_directories(${PROJECT_SOURCE_DIR}/third_party)

aux_source_directory(. SRC_LIST)

add_executable(timer ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} pthread delayqueue threadpool)


