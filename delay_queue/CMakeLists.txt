project(delayqueue)

ADD_DEFINITIONS(-std=c++11)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

aux_source_directory(. SRC_LIST)
message(STATUS "源文件"${SRC_LIST})

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

add_library(${PROJECT_NAME}_static STATIC ${SRC_LIST})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

message(STATUS "到了到了")
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/../third_party)   

# add_executable(delayqueue DelayQueue.cc main.cc)

#add_library(delayqueue SHARED DelayQueue.cc)

target_link_libraries(${PROJECT_NAME} pthread)
