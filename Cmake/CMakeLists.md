# cmake最低版本需求
cmake_minimum_required(VERSION 3.10)

# 工程名称
project (cmake_study)

# 设置C标准还是C++标准
set(CMAKE_C_STANDARD 11)

add_executable(cmake_study
        main.c testFunc.c)


#如果源文件过多，一个一个添加显然不够高效，
#使用aux_source_directory把当前目录下的源文件存列表存放到列表变量里，
#然后在add_executable里调用列表变量。

# 将当前文件夹下的源代码，收集到变量src_path
aux_source_directory(. src_path)

add_executable(cmake_study ${src_path})


#aux_source_directory()也存在弊端，
#它会把指定目录下的所有源文件都加进来，可能会加入一些我们不需要的文件，
#此时我们可以使用set命令新建变量来存放需要的源文件，如下，

set(SRC_LIST
    ./main.c
    ./testFunc1.c
    ./testFunc.c)

add_executable(main ${SRC_LIST})

# 头文件路径
include_directories(include1 include2)

#set命令是用于定义变量的， EXECUTABLE_OUT_PATH 和 PROJECT_SOURCE_DIR 
#是CMake自带的预定义变量，其意义如下，
# EXECUTABLE_OUTPUT_PATH ：可执行文件的存放位置
# PROJECT_SOURCE_DIR ：工程的根目录

#add_subdirectory 表示向当前工程添加存放源文件的子目录，
#当执行cmake时，就会进入src目录下寻找CMakeLists.txt

# 定义变量
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 向当前工程添加存放源文件的子目录
add_subdirectory(src)


# 对 源文件变量 生成动态库 testFunc_shared
add_library(testFunc_shared SHARED ${SRC_LIST})
# 对 源文件变量 生成静态库 testFunc_static
add_library(testFunc_static STATIC ${SRC_LIST})

# 设置最终生成的库的名称
set_target_properties(testFunc_shared PROPERTIES OUTPUT_NAME "testFunc")
set_target_properties(testFunc_static PROPERTIES OUTPUT_NAME "testFunc")

# 设置库文件的输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 把目标文件与库文件进行链接
target_link_libraries(main ${TESTFUNC_LIB})

# 在指定路径下查找库，并把库的绝对路径存放到变量里
find_library(TESTFUNC_LIB testFunc HINTS ${PROJECT_SOURCE_DIR}/testFunc/lib)


find_library：在指定目录下查找指定库，并把库的绝对路径存放到变量里。会查找库是否存在，
第一个参数：是变量名称
第二个参数：是库名称
第三个参数：是HINTS，提示
第4个参数：是路径
其它用法可以参考cmake文档
target_link_libraries：把目标文件与库文件进行链接