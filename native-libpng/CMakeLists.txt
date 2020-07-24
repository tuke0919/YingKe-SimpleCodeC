
cmake_minimum_required(VERSION 3.4.1)

## 官方标准配置
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

# 引入三方库

# 1, 包含头文件
include_directories(${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/include_png)
include_directories(${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/include_zlib)

# 2，包含so
add_library(libpng
        SHARED
        IMPORTED)

set_target_properties(libpng
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libpng.so
        )

add_library(libz
        SHARED
        IMPORTED)

set_target_properties(libz
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libz.so
        )

# 生成目标库，链接三方库
# 头文件
include_directories(src/main/cpp)

add_library(yingke-libpng
           SHARED
           src/main/cpp/libpng_util.cpp)

target_link_libraries(yingke-libpng
            libpng
            libz
            android
            jnigraphics
            log)
