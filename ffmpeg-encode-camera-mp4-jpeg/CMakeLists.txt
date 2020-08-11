
cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/cpp)
add_library(yingke-camera
        SHARED
        src/main/cpp/camera_encoder.cpp
        src/main/cpp/encode_mp4.cpp
        src/main/cpp/encode_jpeg.cpp
        )

# 获取上级 目录
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
set(LIBRARY_DIR ${PARENT_DIR}/ffmpeg_single)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

# 引入三方库 ffmpeg
add_library(ffmpeg_single
        SHARED
        IMPORTED
        )
set_target_properties(
        ffmpeg_single
        PROPERTIES IMPORTED_LOCATION
        ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libffmpeg.so)

# ffmpeg头文件
include_directories(${LIBRARY_DIR}/libs/${ANDROID_ABI}/include)

# 引入三方库 libyuv
set(LIBYUV_DIR ${PARENT_DIR}/third-libyuv)

add_library(lib_yuv
        SHARED
        IMPORTED
        )

set_target_properties(
        lib_yuv
        PROPERTIES IMPORTED_LOCATION
        ${LIBYUV_DIR}/libs/${ANDROID_ABI}/libyuv.so
)

# libyuv库头文件
include_directories(${LIBYUV_DIR}/libs/${ANDROID_ABI}/include)

find_library(
        log-lib
        log)

# 链接
target_link_libraries(
        yingke-camera
        ffmpeg_single
        lib_yuv
        ${log-lib})