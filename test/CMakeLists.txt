cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/3rd/inc)
include_directories(${CMAKE_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/src)

enable_testing()

# Add test files
file(GLOB test_files "*.test.cc")
foreach (test_file ${test_files})
    get_filename_component(test_name ${test_file} NAME_WLE)
    add_executable(${test_name} ${test_file})
    target_compile_definitions(${test_name} PRIVATE CATCH_CONFIG_MAIN)
    target_compile_definitions(${test_name} PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING)
    add_test(NAME ${test_name} COMMAND ${test_name})
    target_link_libraries (${test_name} PRIVATE Threads::Threads)
endforeach ()
