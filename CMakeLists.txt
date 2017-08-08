cmake_minimum_required(VERSION 2.8)

# Get project name from top directory
string(REPLACE "/" ";" DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})
list(REVERSE DIR_LIST)
list(GET DIR_LIST 0 PROJ_NAME)

project(${PROJ_NAME})

SET(CMAKE_CXX_FLAGS "-std=c++0x -Wfatal-errors")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJ_NAME} main.cpp
                            CalData.cpp
                            ConfigParser.cpp
                            TextureConfigData.cpp)

target_link_libraries(${PROJ_NAME} 
                        ${OpenCV_LIBS})
