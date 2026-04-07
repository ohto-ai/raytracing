cmake_minimum_required (VERSION 3.10)

# CUDA is the sole rendering back-end.
# Set ENABLE_CUDA=ON when a CUDA toolkit and NVIDIA GPU are available.
option(ENABLE_CUDA "Enable CUDA rendering (requires NVIDIA GPU + CUDA toolkit)" OFF)

if(ENABLE_CUDA)
    project(raytracing LANGUAGES CXX CUDA)
else()
    project(raytracing LANGUAGES CXX)
endif()

find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(ENABLE_CUDA)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)

    add_executable(${PROJECT_NAME} src/main.cc src/render.cu)

    if(WIN32)
      target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    endif()
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        SPDLOG_FMT_EXTERNAL
        FMT_HEADER_ONLY
        OHTOAI_ENABLE_CUDA
    )

    target_include_directories(${PROJECT_NAME} PRIVATE 3rd/inc inc)

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(ARCH x64)
    else()
      set(ARCH x86)
    endif()
    target_link_directories(${PROJECT_NAME} PRIVATE 3rd/lib/${ARCH} 3rd/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

    # Target all common GPU architectures (Volta / Turing / Ampere / Ada / Hopper)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CUDA_ARCHITECTURES "all-major"
    )
endif()

add_subdirectory(test)
