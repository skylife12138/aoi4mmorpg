# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

#项目名称, 参数值是 Demo1, 该命令表示项目的名称是 Demo1
project(Aoi)

set(CMAKE_BUILD_TYPE Release CACHE STRING "set build type to Release")

set(BUILD_FLAGS "${BUILD_FLAGS} -Wall -g")


###  指定c++文件时编译选项
set(CMAKE_CXX_FLAGS ${BUILD_FLAGS})
add_definitions(-DNDEBUG)
add_definitions(-D_USE_PREC)
add_definitions(-DX64)
add_definitions(-D_REENTRANT)
add_definitions(-DLUA_USE_LINUX)

### 指定库头文件的搜索路径
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})


### 添加源代码
set(src_root
  define.h
  common.h
  common.cpp
  all.h
  all.cpp
  light.h
  light.cpp
  nine.h
  nine.cpp
  link.h
  link.cpp
  main.cpp
)

set(SOURCE_FILES
  ${src_root}
)

### 指定生成的可执行文件目录
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build) 
# 指定生成目标
add_executable(Aoi ${SOURCE_FILES})
