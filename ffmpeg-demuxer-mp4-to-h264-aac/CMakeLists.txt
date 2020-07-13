
cmake_minimum_required(VERSION 3.4.1)

# 包含头文件
include_directories(src/main/cpp)
add_library(yingke-mp4-to-h264-aac
        SHARED
        src/main/cpp/Mp4DemuxerUtil.cpp
        src/main/cpp/ffmpeg_demuxer.cpp
        )

# 获取上级 目录
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
set(LIBRARY_DIR ${PARENT_DIR}/ffmpeg_single)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

# 设置so动态库输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src/main/output/${ANDROID_ABI})

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

find_library(
        log-lib
        log)

# 链接
target_link_libraries(
        yingke-mp4-to-h264-aac
        ffmpeg_single
        ${log-lib})