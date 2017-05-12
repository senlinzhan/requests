cmake_minimum_required (VERSION 2.8)

project(requests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

find_package(Boost COMPONENTS system REQUIRED)

set(SUBDIRS src examples)
 
foreach(dir ${SUBDIRS})
    add_subdirectory(${dir})
endforeach()
