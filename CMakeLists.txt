

cmake_minimum_required(VERSION 3.8)

MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR}/repo )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} " ${CMAKE_SOURCE_DIR}/repo/")
MESSAGE( STATUS "CMAKE_MODULE_PATH:         " ${CMAKE_MODULE_PATH} )

project(pNes)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
include(${CMAKE_SOURCE_DIR}/repo/FindSDL2/FindSDL2.cmake)
include(${CMAKE_SOURCE_DIR}/repo/FindSDL2/FindSDL2_ttf.cmake)

add_subdirectory(src)
add_subdirectory(test)