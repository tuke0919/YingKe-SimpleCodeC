
cmake_minimum_required(VERSION 3.4.1)

## 官方标准配置
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

# 引入三方库

# 获取父路径
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
set(LIB_PNG_DIR ${PARENT_DIR}/third-libpng)
set(LIB_JPEG_TURBO_DIR ${PARENT_DIR}/third-libjpeg-turbo)

########################引入jpeg库############################
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

########################引入png库############################

# 1, 包含头文件
include_directories(${LIB_PNG_DIR}/libs/${ANDROID_ABI}/include_png)
include_directories(${LIB_PNG_DIR}/libs/${ANDROID_ABI}/include_zlib)

# 2，包含so
add_library(libpng
        SHARED
        IMPORTED)

set_target_properties(libpng
        PROPERTIES IMPORTED_LOCATION
        ${LIB_PNG_DIR}/libs/${ANDROID_ABI}/libpng.so
        )

add_library(libz
        SHARED
        IMPORTED)

set_target_properties(libz
        PROPERTIES IMPORTED_LOCATION
        ${LIB_PNG_DIR}/libs/${ANDROID_ABI}/libz.so
        )

# 生成目标库，链接三方库
# 头文件
include_directories(src/main/cpp)

# 添加多个cpp文件
file(GLOB NATIVE_SRC_CPP "src/main/cpp/*.cpp")
add_library(yingke-yuv-to-jpeg-png
        SHARED
        src/main/cpp/yuv_to_jpeg_png_util.cpp
        src/main/cpp/yuv_to_jpeg_util.cpp
        src/main/cpp/yuv_to_png_util.cpp
        src/main/cpp/yuv_to_rgb_util.cpp)

target_link_libraries(yingke-yuv-to-jpeg-png
        libjpeg
        libjpeg-turbo
        libpng
        libz

        android
        jnigraphics
        log)
