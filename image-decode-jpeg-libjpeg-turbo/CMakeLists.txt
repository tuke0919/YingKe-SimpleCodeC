
cmake_minimum_required(VERSION 3.4.1)

## 官方标准配置
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

# 引入三方库

get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)

set(LIB_JPEG_TURBO_DIR ${PARENT_DIR}/third-libjpeg-turbo)
# 1, 包含头文件
include_directories(${LIB_JPEG_TURBO_DIR}/libs/${ANDROID_ABI}/include_jpeg)

# 2，包含so
add_library(libjpeg
        SHARED
        IMPORTED)

set_target_properties(libjpeg
        PROPERTIES IMPORTED_LOCATION
        ${LIB_JPEG_TURBO_DIR}/libs/${ANDROID_ABI}/libjpeg.so
        )

add_library(libjpeg-turbo
        SHARED
        IMPORTED)

set_target_properties(libjpeg-turbo
        PROPERTIES IMPORTED_LOCATION
        ${LIB_JPEG_TURBO_DIR}/libs/${ANDROID_ABI}/libturbojpeg.so
        )

# 生成目标库，链接三方库
# 头文件
include_directories(src/main/cpp)

add_library(yingke-libjpeg-turbo
           SHARED
           src/main/cpp/libjpegturbo_util.cpp)

target_link_libraries(yingke-libjpeg-turbo
            libjpeg
            libjpeg-turbo
            android
            jnigraphics
            log)
