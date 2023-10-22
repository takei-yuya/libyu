cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(libyu CXX)

enable_testing()

add_subdirectory(sample)
add_subdirectory(test)
add_subdirectory(app)
