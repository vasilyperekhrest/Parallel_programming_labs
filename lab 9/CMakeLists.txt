cmake_minimum_required(VERSION 3.20)
project(lab_9)

set(CMAKE_CXX_STANDARD 20)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.50 REQUIRED COMPONENTS filesystem)

include_directories (${Boost_INCLUDE_DIR})

add_subdirectory(l1)
add_subdirectory(l2)
add_subdirectory(lab\ 8)
