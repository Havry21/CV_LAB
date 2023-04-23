cmake_minimum_required(VERSION 3.16)

project(Lab6 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)


add_executable(Lab6 main.cpp)

target_link_libraries (Lab6 PUBLIC ${OpenCV_LIBS})
