cmake_minimum_required(VERSION 3.20)

project(mock_camera)
add_subdirectory(libjr_util)
add_subdirectory(libjr_socket)
add_executable(mock_camera mock_camera.c ptz.c)
target_link_libraries(mock_camera PUBLIC jr_util)
target_link_libraries(mock_camera PUBLIC jr_socket)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)