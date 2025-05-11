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

set(LOG_SRC_LIST
    ./src/logger.cpp)

add_library(LOGGER_STATIC_LIB STATIC ${LOG_SRC_LIST})

set_target_properties(LOGGER_STATIC_LIB PROPERTIES OUTPUT_NAME "logger.a")


#设置源文件目录
aux_source_directory(${PROJECT_SOURCE_DIR}/test/ SRC_LIST)

#设置可执行文件
add_executable(logger_test ${SRC_LIST})

#链接库
target_link_libraries(logger_test LOGGER_STATIC_LIB)

