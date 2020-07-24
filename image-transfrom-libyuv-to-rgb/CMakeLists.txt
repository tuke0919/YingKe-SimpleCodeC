
cmake_minimum_required(VERSION 3.4.1)

## 官方标准配置
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

# 获取上级目录
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
# 引入三方库

# 1, 包含头文件
set(LIB_YUV_DIR ${PARENT_DIR}/third-libyuv)
include_directories(${LIB_YUV_DIR}/libs/${ANDROID_ABI}/include)

# 2，包含so
add_library(libyuv
        SHARED
        IMPORTED)

set_target_properties(libyuv
        PROPERTIES IMPORTED_LOCATION
        ${LIB_YUV_DIR}/libs/${ANDROID_ABI}/libyuv.so
        )
# 生成目标库，链接三方库
# 头文件
include_directories(src/main/cpp)

add_library(yingke-libyuv2rgb
           SHARED
           src/main/cpp/native_libyuv2rgb.cpp)

target_link_libraries(yingke-libyuv2rgb
            libyuv
            android
            jnigraphics
            log)
