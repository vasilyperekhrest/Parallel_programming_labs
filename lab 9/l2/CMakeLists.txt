cmake_minimum_required(VERSION 3.20)
project(lab_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab_2 main.cpp)

target_link_libraries(lab_2 ${Boost_LIBRARIES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/modules"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/modules)