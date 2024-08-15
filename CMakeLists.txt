#设置最低版本要求
cmake_minimum_required(VERSION 3.0)
#将c++表准设置为c++11
set(CMAKE_CXX_STANDARD 11)
#项目名称
project(SearchEngine)
#设置项目源文件所在地
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
#从项目中目录收集源文件
aux_source_directory(${SRC_DIR} MAIN_SRC_LIST)

#单独收集字典库生成的可执行文件所需要的源文件
set(DICT_SRC_LIST 
    ${SRC_DIR}/Configuration.cc 
    ${SRC_DIR}/CppJieba.cc
    ${SRC_DIR}/DictProducer.cc
    ${SRC_DIR}/TestDict.cc
    ${SRC_DIR}/Mylogger.cc
    )
add_executable(DictProducer ${DICT_SRC_LIST})


#启动网页查询离线阶段程序
set(WEB_SRC_LIST 
    ${SRC_DIR}/Configuration.cc 
    ${SRC_DIR}/CppJieba.cc
    ${SRC_DIR}/TestPage.cc
    ${SRC_DIR}/Mylogger.cc
    ${SRC_DIR}/PageLib.cc
    ${SRC_DIR}/WebPage.cc
    ${SRC_DIR}/PageLibprocessor.cc
    ${PROJECT_SOURCE_DIR}/include/tinyxml2/tinyxml2.cpp
    )
add_executable(WebProducer ${WEB_SRC_LIST})

# 从 MAIN_SRC_LIST 中移除 DICT_SRC_LIST 中含有main函数的文件
list(REMOVE_ITEM MAIN_SRC_LIST ${SRC_DIR}/TestDict.cc ${SRC_DIR}/TestPage.cc)
# 
#从项目中目录中搜集头文件
include_directories(${PROJECT_SOURCE_DIR}/include)
#第三方库
include_directories(${PROJECT_SOURCE_DIR}/include/tinyxml2 ${PROJECT_SOURCE_DIR}/include/simhash)

#设置可生成文件目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)
#从收集的源文件创建可执行文件
add_executable(server ${MAIN_SRC_LIST})


#连接必要的库
target_link_libraries(server log4cpp pthread  ${PROJECT_SOURCE_DIR}/lib/libtinyxml2.a)
target_link_libraries(DictProducer log4cpp pthread ${PROJECT_SOURCE_DIR}/lib/libtinyxml2.a)
target_link_libraries(WebProducer log4cpp pthread ${PROJECT_SOURCE_DIR}/lib/libtinyxml2.a)
# 添加-g选项给server目标
target_compile_options(server PRIVATE -g)
