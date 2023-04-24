cmake_minimum_required(VERSION 3.16)

project(calibrateCamera LANGUAGES CXX)
project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(calibrateCamera calibrate_camera.cpp)
target_link_libraries (calibrateCamera PUBLIC ${OpenCV_LIBS})

add_executable(main main.cpp)
target_link_libraries (main PUBLIC ${OpenCV_LIBS})
