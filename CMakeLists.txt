cmake_minimum_required(VERSION 3.12)

project(GFGraph)

set(Thrust_DIR "./third_party/thrust/cmake/")

find_package(OpenMP REQUIRED)
find_package(Thrust REQUIRED CONFIG)


set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_BUILD_TYPE RELEASE)
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
