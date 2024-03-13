cmake_minimum_required(VERSION 3.22)
project(BeginningEngineDebugMemInfo_demo_Linux)

set(CMAKE_CXX_STANDARD 20)

add_executable(BeginningEngineDebugMemInfo_demo_Linux main.cpp BeginningEngineDebugMemInfo.h)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 查找 libdw 库
find_library(LIBDW NAMES dw)
if(NOT LIBDW)
    message(FATAL_ERROR "libdw not found")
endif()


set(BEGINNING_ENGINE_DEBUG_MEMINFO_LIB_PATH "${CMAKE_SOURCE_DIR}/libBeginningEngineDebugMemInfo.a")

target_include_directories(BeginningEngineDebugMemInfo_demo_Linux PRIVATE /usr/include/elfutils/)

# 链接静态库和 libdw
target_link_libraries(BeginningEngineDebugMemInfo_demo_Linux PRIVATE ${BEGINNING_ENGINE_DEBUG_MEMINFO_LIB_PATH} ${LIBDW})