cmake_minimum_required(VERSION 3.20)
project(lab_8)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab_8 main.cpp)

target_link_libraries(lab_8 ${Boost_LIBRARIES})