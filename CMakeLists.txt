cmake_minimum_required(VERSION 3.20)

project(mock_camera)

add_executable(mock_camera mock_camera.c ptz.c jr_socket.c)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# include(FindPkgConfig)
# pkg_search_module(OPUSFILE REQUIRED opusfile)
# include_directories(${OPUSFILE_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} ${OPUSFILE_LIBRARIES})
set(CMAKE_BUILD_TYPE Debug)