include(FetchContent)

FetchContent_Declare(
    GTest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(GTest)

file(GLOB allTests "*.cpp")
foreach (test ${allTests})
    get_filename_component(testExe ${test} NAME_WE)
    add_executable(${testExe} ${test})
    target_link_libraries(${testExe} GTest::gtest_main)
    target_include_directories(${testExe} PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
endforeach ()

