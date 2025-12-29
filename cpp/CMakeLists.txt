cmake_minimum_required(VERSION 3.30)

add_library(bytes INTERFACE)
target_include_directories(bytes INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(bytes INTERFACE cxx_std_23)
target_compile_options(bytes INTERFACE -Wall -Werror)

if (BUILD_EXAMPLES)
    add_executable(example examples.cpp)
    target_link_libraries(example PRIVATE bytes)
endif()

if (BUILD_TESTING)
    add_subdirectory(test)
endif()