# 工程名称
project (ysh_c++)

cmake_minimum_required(VERSION 3.22)

#使能c++11
#Enable c++11
set(CMAKE_CXX_STANDARD 11)

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

#设置头文件目录
include_directories(
        ${PROJECT_SOURCE_DIR}/src
)

set(UTIL_SRC_LIST
    ./src/util/logger.cpp
    ./src/util/util.cpp)

set(THREAD_SRC_LIST
    ./src/Thread/TaskExecutor.cpp)


add_library(UTIL_STATIC_LIB STATIC ${UTIL_SRC_LIST})
add_library

set_target_properties(UTIL_STATIC_LIB PROPERTIES OUTPUT_NAME "common.a")


#设置源文件目录
#aux_source_directory(${PROJECT_SOURCE_DIR}/test/ SRC_LIST)
set(LOGGER_TEST_LIST ${PROJECT_SOURCE_DIR}/test/logger_test.cpp)


#设置可执行文件
add_executable(logger_test ${LOGGER_TEST_LIST})

#链接库
target_link_libraries(logger_test UTIL_STATIC_LIB)


#增加可执行文件
#add_executable(logger_test2 ${SRC_LIST})
#target_link_libraries(logger_test2 UTIL_STATIC_LIB)
