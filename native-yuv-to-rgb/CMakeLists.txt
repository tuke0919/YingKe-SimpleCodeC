
cmake_minimum_required(VERSION 3.4.1)

##官方标准配置
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

# 头文件
include_directories(src/main/cpp)

add_library(yingke-yuv2rgb
           SHARED
           src/main/cpp/native_yuv2rgb.cpp)

target_link_libraries(yingke-yuv2rgb
            android
            jnigraphics
            log)
