cmake_minimum_required (VERSION 3.10)

project(raytracing LANGUAGES CXX )

set(CMAKE_CXX_STANDARD 17)

# Add source files
set(SOURCES main.cc)

# Build the executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 3rd/inc)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH x64)
else()
  set(ARCH x86)
endif()
target_link_directories(${PROJECT_NAME} PRIVATE 3rd/lib/${ARCH})
target_link_directories(${PROJECT_NAME} PRIVATE 3rd/lib)


