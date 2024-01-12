cmake_minimum_required (VERSION 3.10)

project(raytracing LANGUAGES CXX )

find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source files
set(SOURCES src/main.cc)

# Build the executable
add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
  add_compile_definitions(WIN32_LEAN_AND_MEAN)
  add_compile_definitions(NOMINMAX)
endif(WIN32)
add_compile_definitions(SPDLOG_FMT_EXTERNAL)
add_compile_definitions(FMT_HEADER_ONLY)

target_include_directories(${PROJECT_NAME} PRIVATE 3rd/inc)
target_include_directories(${PROJECT_NAME} PRIVATE inc)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH x64)
else()
  set(ARCH x86)
endif()
target_link_directories(${PROJECT_NAME} PRIVATE 3rd/lib/${ARCH})
target_link_directories(${PROJECT_NAME} PRIVATE 3rd/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

add_subdirectory(test)
