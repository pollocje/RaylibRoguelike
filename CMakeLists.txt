cmake_minimum_required(VERSION 3.15)
project(semGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Raylib paths
set(RAYLIB_INCLUDE "C:/raylib/raylib/src")
set(RAYLIB_LIB "C:/raylib/raylib/src/libraylib.a")

include_directories(${RAYLIB_INCLUDE})

add_executable(semGame src/main.cpp)

target_link_libraries(semGame ${RAYLIB_LIB} opengl32 gdi32 winmm)

target_include_directories(semGame PRIVATE ${RAYLIB_INCLUDE})
