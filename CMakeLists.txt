cmake_minimum_required(VERSION 3.12)

project(GFGraph)

set(Thrust_DIR "./third_party/thrust/cmake/")

find_package(OpenMP REQUIRED)
find_package(Thrust REQUIRED CONFIG)

# 设置pybind11的路径
set(PYBIND11_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pybind11)
set(PYBIND11_INCLUDE_DIR ${PYBIND11_ROOT}/include)

# 添加pybind11的CMake模块路径
list(APPEND CMAKE_MODULE_PATH "${PYBIND11_ROOT}/tools")

# 包含pybind11
include_directories(${PYBIND11_INCLUDE_DIR})
find_package(pybind11 CONFIG PATHS ${PYBIND11_ROOT} NO_DEFAULT_PATH)
if(NOT pybind11_FOUND)
    add_subdirectory(${PYBIND11_ROOT})
endif()

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "BuildType: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall")

include_directories(third_party)
include_directories(third_party/cuda)
include_directories(include)

link_libraries(OpenMP::OpenMP_CXX)
thrust_create_target(ThrustOMP HOST CPP DEVICE OMP)
link_libraries(ThrustOMP)

add_subdirectory(tests)

# 添加 Python 模块
pybind11_add_module(pyGFGraph src/pyGFGraph.cpp)  # 新增 Python 接口源文件
target_link_libraries(pyGFGraph PRIVATE OpenMP::OpenMP_CXX ThrustOMP)