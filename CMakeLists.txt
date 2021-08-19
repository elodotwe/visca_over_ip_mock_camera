cmake_minimum_required(VERSION 3.20)

project(mock_camera)
add_subdirectory(libjr_util)
add_executable(mock_camera mock_camera.c ptz.c jr_socket.c)
target_link_libraries(mock_camera PUBLIC jr_util)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)