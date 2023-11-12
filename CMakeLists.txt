cmake_minimum_required(VERSION 3.21)
project(ada_a3_image_conv)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(ada_a3_image_conv main.cpp App.cpp App.h)

target_link_libraries(ada_a3_image_conv OpenMP::OpenMP_CXX)
target_link_libraries(ada_a3_image_conv ${OpenCV_LIBS})
target_link_libraries(ada_a3_image_conv ${SDL2_LIBRARIES})
