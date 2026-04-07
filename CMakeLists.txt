cmake_minimum_required (VERSION 3.10)

# Detect CUDA before project() so the LANGUAGES line is correct
option(ENABLE_CUDA "Enable CUDA rendering support" OFF)

if(ENABLE_CUDA)
    project(raytracing LANGUAGES CXX CUDA)
else()
    project(raytracing LANGUAGES CXX)
endif()

find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source files
set(SOURCES src/main.cc src/render.cpp)

if(ENABLE_CUDA)
    list(APPEND SOURCES src/render.cu)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    # Allow the CUDA compiler to use C++ host compiler for host-side code
    set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
endif()

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

if(ENABLE_CUDA)
    target_compile_definitions(${PROJECT_NAME} PRIVATE OHTOAI_ENABLE_CUDA)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CUDA_ARCHITECTURES "all-major"
    )
endif()

add_subdirectory(test)
