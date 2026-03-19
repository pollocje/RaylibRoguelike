cmake_minimum_required(VERSION 3.15)
project(semGame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Put exe in the build folder root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Get raylib automatically
include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

# Grab all cpp files in src
file(GLOB SOURCES "src/*.cpp")

add_executable(semGame ${SOURCES})

target_include_directories(semGame PRIVATE src)

target_link_libraries(semGame PRIVATE raylib)

# Windows extras
if (WIN32)
    target_link_libraries(semGame PRIVATE winmm)
endif()
