cmake_minimum_required(VERSION 3.26)
project(ada_a3_image_conv)

set(CMAKE_CXX_STANDARD 20)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ada_a3_image_conv main.cpp)

target_link_libraries(ada_a3_image_conv ${OpenCV_LIBS})
