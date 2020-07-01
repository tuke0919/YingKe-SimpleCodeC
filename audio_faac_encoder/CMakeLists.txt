
cmake_minimum_required(VERSION 3.4.1)


# 导入第三方静态库
set(LIB_FAAC_DIR ${CMAKE_SOURCE_DIR}/libs)

add_library(faac
        STATIC
        IMPORTED)

set_target_properties(faac
        PROPERTIES
        IMPORTED_LOCATION ${LIB_FAAC_DIR}/${ANDROID_ABI}/libfaac.a)

# 包含头文件
include_directories(src/main/cpp/include)
# 设置so动态库输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src/main/output/${ANDROID_ABI})
# 添加库
add_library(yingke-faac
        SHARED
        src/main/cpp/faac_util.cpp)
# 搜索库
find_library( # 设置path变量的名称
        log-lib
        # 在CMake定位前指定的NDK库名称
        log )
# 链接库
target_link_libraries(yingke-faac
        faac
        ${log-lib})