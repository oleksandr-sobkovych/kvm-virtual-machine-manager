cmake_minimum_required(VERSION 3.16)
project(lab_second_integral)

find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)


add_library(integral SHARED functions.cpp Configuration.cpp Integral.cpp time_measure.cpp error.h config.h Integral.h time_measure.h functions.h)
add_executable(lab_second_integral main.cpp)
target_link_libraries(lab_second_integral integral)
target_link_libraries(lab_second_integral Threads::Threads)