project(delayqueuemain)

add_definitions(-std=c++11)

include_directories(${PROJECT_SOURCE_DIR}/..)

link_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME}  delayqueue pthread)