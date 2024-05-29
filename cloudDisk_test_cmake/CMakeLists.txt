#设置最低版本要求
cmake_minimum_required(VERSION 3.0)
#项目名称
project(cloudDisk)

# 从项目目录收集源文件
aux_source_directory(${PROJECT_SOURCE_DIR}/src MAIN_SRC_LIST)
#移除与生成main无关的文件
list(REMOVE_ITEM MAIN_SRC_LIST ${PROJECT_SOURCE_DIR}/src/consumer.cc)
# 将C++标准设置为11
set(CMAKE_CXX_STANDARD 11)
#设置生成consumer所需要的文件，若问价数少可以用这个
set(CONSUMER ${PROJECT_SOURCE_DIR}/src/consumer.cc)
#设置生成可执行文件目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)
# 从收集的源文件创建可执行文件
add_executable(server ${MAIN_SRC_LIST})
add_executable(consumer ${CONSUMER})

# 链接必要的库
target_link_libraries(server 
    SimpleAmqpClient 
    wfrest
    ssl
    crypt
    crypto
    alibabacloud-oss-cpp-sdk
    curl
    pthread
    alibabacloud-sdk-core
)

target_link_libraries(consumer
    SimpleAmqpClient
    alibabacloud-oss-cpp-sdk
    curl
    crypto
    pthread
    )
# 应用编译选项
target_compile_options(server PRIVATE -fno-rtti)
target_compile_options(consumer PRIVATE -fno-rtti)

