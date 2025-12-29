cmake_minimum_required(VERSION 3.30)
project(bit_viewer LANGUAGES CXX)

option(BUILD_EXAMPLES "Build Examples" OFF)
option(BUILD_TESTING "Build Testing" ON)

if (BUILD_TESTING)
    enable_testing()
endif()

add_subdirectory(cpp)
add_subdirectory(py)